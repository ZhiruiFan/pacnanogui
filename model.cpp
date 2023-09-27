/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Model.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 19th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "model.h"

#include "ui_model.h"

/*  ############################################################################
 *  constructor:  create the model object   */
Model::Model(QWidget* parent, QString*& projName, QString*& workPath)
    : QDialog(parent), ui(new Ui::Model) {
    //  setup the UI interface
    ui->setupUi(this);

    //  assgin the path information
    project = projName;
    workDir = workPath;

    //  create a new property model
    index  = 0;
    proNew = new ModelProperty(index, project, workDir);

    //  other moduled environment
    setupSourceSelect();  // the source selection
    setupMessageBox();    // the message box
    setupDialogButton();  // ok and cancel button
    setupManager();       // the model manager
    setupRenameDialog();  // rename dialog
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor: destroy the Model object  */
Model::~Model() {
    delete ui;
    delete msgbox;
    delete openModel;
    delete openIo;
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setupSourceSelect: setup the environment for selecting a source type
 *  to create a Model */
void Model::setupSourceSelect() {
    //  create the dialog for model creation
    openModel = new Open(this, 3);
    openIo    = new Open(this, 5);

    //  set the default model source
    ui->useNew->setChecked(true);

    //  link the open folder
    connect(ui->btnOpen, &QPushButton::clicked, this, [&]() {
        if (ui->useNew->isChecked())
            openIo->exec();
        else
            openModel->exec();
    });

    //  connect the OPEN dialog of create New
    connect(openIo, &QDialog::accepted, this, [&]() {
        openIo->getSelectContent(text);
        ui->source->setText(text);
    });

    //  connect the OPEN dialog of using old
    connect(openModel, &QDialog::accepted, this, &Model::loadModelDatabase);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setupMessageBox: setup the environment of the Message Box  */
void Model::setupMessageBox() {
    //  create the message box
    msgbox = new MessageBox(this);

    //  connect the YES and NO button of the MessageBox
    connect(msgbox, &QDialog::accepted, this,
            [&]() { proNew->ifOverride = true; });

    //  connect the NO button of the MessageBox
    connect(msgbox, &QDialog::rejected, this,
            [&]() { proNew->ifOverride = false; });
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setupDialogButton: setup the button of in the dialog  */
void Model::setupDialogButton() {
    //  Calcel button
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
    //  OK button
    connect(ui->btnOk, &QPushButton::clicked, this, [&]() {
        //  check the completation of the input
        if (confimDialog()) {
            //  save the dialog information to the object
            saveModelDialog();
            //  close the dialog
            accept();
        }
    });
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setupManager: setup the model manager object  */
void Model::setupManager() {
    //  model list view
    itemModel = new QStandardItemModel(this);
    //  create the manager
    mng = new Manager(this, "Model Manager", ":/icons/material.png", itemModel);
    //  assign the active flag
    isActiveMng = false;
    //  connect the signal and button
    connect(mng, &Manager::signalCreate, this, &Model::createModel);
    connect(mng, &Manager::signalEdit, this, &Model::loadModelDialog);
    connect(mng, &Manager::signalRename, this, &Model::getModelName);
    connect(mng, &Manager::signalDelete, this, &Model::deleteModel);
    connect(mng, &Manager::shown, this, [&]() { isActiveMng = true; });
    connect(mng, &Manager::closed, this, [&]() { isActiveMng = false; });
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setup Rename dialog  */
void Model::setupRenameDialog() {
    //  rename dialog
    rename = new Rename(this);
    //  connect OK button of the dialog
    connect(rename, &QDialog::accepted, this, &Model::setModelName);
}

/*  ############################################################################
 *  createModel: create a new model  */
void Model::createModel() {
    //  disable the edit mode
    editmode = false;

    //  set the model name
    ui->name->setText(*proNew->name);

    //  set the project that will be attached to
    ui->project->setText(*project);

    //  clear the source information
    ui->source->clear();

    //  initialize the dialog interface
    ui->model2d->setChecked(true);  // modeling type
    ui->useNew->setChecked(true);   // source type
    ui->useHdf5->setChecked(true);  // source data type

    //  show the MODEL dialog
    exec();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  getModelList: get the model list, that will be used to set the model list in
 *  Mainwindows*/
QStandardItemModel* Model::getModelList() { return itemModel; }

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  set currently operated model  */
void Model::setOperatedModel(int idx) {
    if (idx >= 0) {
        //  get the current model index
        item = itemModel->item(idx, 0);
        //  get the current model
        proCur = item->data(Qt::UserRole).value<ModelProperty*>();
        //  update the set object
        set = proCur->set;
    } else {
        set = proNew->set;
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  saveModelDialog: save the dialog information to the variables  */
void Model::saveModelDialog() {
    //  assign the temporary model property
    proTem = editmode ? proOld : proNew;

    //  name of model
    *proTem->name = ui->name->text();

    //  modeling type
    if (ui->model2d->isChecked()) proTem->modelType = 0;
    if (ui->model3d->isChecked()) proTem->modelType = 1;

    //  the name of the source
    proTem->source = ui->source->text();

    //  source type
    if (ui->useAscii->isChecked()) proTem->sourceType = 0;
    if (ui->useBin->isChecked()) proTem->sourceType = 1;
    if (ui->useHdf5->isChecked()) proTem->sourceType = 2;

    //  creation type
    if (ui->useNew->isChecked()) proTem->createType = 0;
    if (ui->useOld->isChecked()) proTem->createType = 1;

    //  update the model name if in edit mode
    if (editmode) {
        //  update the name of the item
        item->setText(*proTem->name);
    }
    //  add the created model to the list view
    else {
        //  create the item
        item = new QStandardItem(*proTem->name);
        item->setData(QVariant::fromValue(proTem), Qt::UserRole);
        itemModel->appendRow(item);
        //  assign the item to null and create the new property object
        item   = nullptr;
        proNew = nullptr;
        ++index;
        proNew = new ModelProperty(index, project, workDir);
    }
    //  assign the temporary ModelProperty object to null
    proTem = nullptr;
    //  show the mananger dialog if possible
    if (isActiveMng) mng->show();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  confirmDialog: check the dialog information is complete  */
bool Model::confimDialog() {
    //  the name of the model
    if (ui->name->text() == "") {
        msgbox->showMessage(1, ":/icons/model.png", "Model",
                            "Please input the \"name\" of the model.");
        return false;
    }
    //  the input source
    if (ui->source->text() == "") {
        msgbox->showMessage(1, ":/icons/model.png", "Model",
                            "Please input the \"source\" of the model.");
        return false;
    }
    //  all check is done
    return true;
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  loadModelDialog: load the model information to the dialog  */
void Model::loadModelDialog(int idx) {
    //  assign the edit mode
    editmode = true;
    //  get the current property
    item   = itemModel->item(idx, 0);
    proOld = item->data(Qt::UserRole).value<ModelProperty*>();
    //  set the name of the model
    ui->name->setText(*proOld->name);

    //  set the model type
    switch (proOld->modelType) {
        case 0:
            ui->model2d->setChecked(true);
            break;
        case 1:
            ui->model3d->setChecked(true);
            break;
    }

    //  set the source
    ui->source->setText(proOld->source);

    //  set the source data type
    switch (proOld->sourceType) {
        case 0:
            ui->useAscii->setChecked(true);
            break;
        case 1:
            ui->useBin->setChecked(true);
            break;
        case 2:
            ui->useHdf5->setChecked(true);
            break;
    }

    //  set the create type
    switch (proOld->createType) {
        case 0:
            ui->useNew->setChecked(true);
            break;
        case 1:
            ui->useOld->setChecked(true);
            break;
    }

    //  set the project name
    ui->project->setText(*project);
    //  show the model dialog
    exec();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  getModelName: get the model name and assign ti to the Rename dialog  */
void Model::getModelName(int idx) {
    //  set the old name
    if (idx >= 0) {
        //  get the item
        item = itemModel->item(idx, 0);
        //  get the current model property in edit
        proOld = item->data(Qt::UserRole).value<ModelProperty*>();
        //  assign the old name
        rename->setNameOriginal(*proOld->name);
        //  show the rename dialog
        rename->exec();
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  getNewName: get the new name of the material  */
void Model::setModelName() {
    //  set the new name of the model
    QString text;
    rename->getNameNew(text);
    *proOld->name = text;
    //  update the name of the current item
    item->setText(text);
    //  rest the pointer
    proOld = nullptr;
    item   = nullptr;

    //  show the Manager dialog if possible
    if (isActiveMng) mng->show();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  delete the model information from the dialog  */
void Model::deleteModel(int idx) {
    //  check the selection is valide or not
    if (idx >= 0) {
        //  get the item
        item = itemModel->item(idx, 0);
        //  get the current model property in edit
        proOld = item->data(Qt::UserRole).value<ModelProperty*>();
        //  remove the item from the list view
        itemModel->removeRow(idx);
        //  remove the property object
        delete proOld;
        proOld = nullptr;
    }
}

/*  ############################################################################
 *  saveModelDatabase: save the basic information of the model database*/
void Model::saveModelDatabase() {
    //  iterate through all items in the list view
    for (int i = 0; i < itemModel->rowCount(); ++i) {
        //  get the item
        item = itemModel->item(i, 0);
        //  get the current model property
        proOld = item->data(Qt::UserRole).value<ModelProperty*>();
        //  save the database
        proOld->saveDatabase(*workDir);
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  loadModel: load the model information from the local database  */
void Model::loadModelDatabase() {
    //  set the database name
    proNew->dbname = ui->source->text();
    //  create the database
    proNew->db.setDatabaseName(proNew->dbname);
    if (!proNew->db.open()) {
        qDebug() << "Failed to open the database"
                 << proNew->db.lastError().text();
    }
    //  load the information of project
    QSqlQuery query(proNew->db);
    query.prepare("SELECT * FROM MODEL;");

    //  perform the query command
    query.exec();

    //  get the name of the model
    query.next();
    *proNew->name = query.value(2).toString();
    qDebug() << proNew->name;

    //  get the model type
    query.next();
    proNew->modelType = query.value(2).toInt();
    qDebug() << proNew->modelType;

    //  get the source
    query.next();
    proNew->source = query.value(2).toString();
    qDebug() << proNew->source;

    //  get the source type
    query.next();
    proNew->sourceType = query.value(2).toInt();
    qDebug() << proNew->sourceType;

    //  assign the creation type
    query.next();
    proNew->createType = 1;
    qDebug() << proNew->createType;

    //  close the query
    query.finish();
    query.clear();

    //  close the database
    proNew->db.close();
}

/*  ############################################################################
 *  Model Property related interface
 *  constructor: create the Model property object */
Model::ModelProperty::ModelProperty(int _index, QString*& projName,
                                    QString*& workPah) {
    //  initialize the basic information
    assigned   = false;
    ifExist    = false;
    ifOverride = false;
    ifCreateDb = false;

    //  assign the model name
    name = new QString("Model-" + QString::number(_index));
    //  assign the project name and work directory
    project = projName;
    workDir = workPah;

    //  create the set object
    set = new Set(nullptr, project, name, workDir);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  saveDatabase: save model information to the local database */
void Model::ModelProperty::saveDatabase(QString& workDir) {
    //  create the database
    if (!ifCreateDb) {
        db = QSqlDatabase::addDatabase("QSQLITE", *name);
    }
}

/*  ############################################################################
 *  show Set create dialog */
void Model::createSetNode() {
    if (itemModel->rowCount() == 0) {
        msgbox->showMessage(0, ":/icons/node.png", "Set creation",
                            "No model is detected. Please create a new one "
                            "before the node set creation can be used.");
    } else {
        set->createSetNode();
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  show Set create dialog  */
void Model::createSetElem() {
    if (itemModel->rowCount() == 0) {
        msgbox->showMessage(0, ":/icons/element.png", "Set creation",
                            "No model is detected. Please create a new one "
                            "before the element set creation can be used.");
    } else {
        set->createSetElem();
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  show Set manager  */
void Model::showSetManager() {
    if (itemModel->rowCount() == 0) {
        msgbox->showMessage(0, ":/icons/set.png", "Set creation",
                            "No model is detected. Please create a new one "
                            "before the set manager can be used.");
    } else {
        set->showManager();
    }
}
