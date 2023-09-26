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
#include "set.h"

#include "ui_set.h"

/*  ############################################################################
 *  constructor:  create the model object   */
Set::Set(QWidget* parent, QString*& projName, QString*& modelName,
         QString*& workPath)
    : QDialog(parent), ui(new Ui::Set) {
    //  setup the UI interface
    ui->setupUi(this);

    //  assgin the path information
    project = projName;
    model   = modelName;
    workDir = workPath;

    //  create a new property model
    index  = 0;
    proNew = new Property(index, project, model, workDir);

    //  other moduled environment
    setupSourceSelect();  // the source selection
    setupMessageBox();    // the message box
    setupDialogButton();  // ok and cancel button
    setupManager();       // the model manager
    setupRenameDialog();  // rename dialog

    //  set the list view of models
    ui->model->setText(*model);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor: destroy the Model object  */
Set::~Set() {
    delete ui;
    delete msgbox;
    delete openIo;
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setupSourceSelect: setup the environment for selecting a source type
 *  to create a Model */
void Set::setupSourceSelect() {
    //  create the dialog for model creation
    openIo = new Open(this, 5);

    //  set the default model source
    ui->useNew->setChecked(true);

    //  link the open folder
    connect(ui->btnOpen, &QPushButton::clicked, openIo, &QDialog::exec);

    //  connect the OPEN dialog of create New
    connect(openIo, &QDialog::accepted, this, [&]() {
        openIo->getSelectContent(text);
        ui->source->setText(text);
    });
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setupMessageBox: setup the environment of the Message Box  */
void Set::setupMessageBox() {
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
void Set::setupDialogButton() {
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
void Set::setupManager() {
    //  model list view
    itemModel = new QStandardItemModel(this);
    //  create the manager
    mng = new Manager(this, "Model Sets", ":/icons/set.png", itemModel);
    //  assign the active flag
    isActiveMng = false;
    //  connect the signal and button
    connect(mng, &Manager::signalCreate, this, &Set::createSetNode);
    connect(mng, &Manager::signalEdit, this, &Set::loadModelDialog);
    connect(mng, &Manager::signalRename, this, &Set::getModelName);
    connect(mng, &Manager::signalDelete, this, &Set::deleteModel);
    connect(mng, &Manager::shown, this, [&]() { isActiveMng = true; });
    connect(mng, &Manager::closed, this, [&]() { isActiveMng = false; });
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setup Rename dialog  */
void Set::setupRenameDialog() {
    //  rename dialog
    rename = new Rename(this);
    //  connect OK button of the dialog
    connect(rename, &QDialog::accepted, this, &Set::setModelName);
}

/*  ############################################################################
 *  createModel: create a new model  */
void Set::createSetNode() {
    //  disable the edit mode
    editmode = false;

    //  set the model name
    ui->name->setText(proNew->name);
    //  set the project that will be attached to
    ui->project->setText(*project);
    ui->project->setReadOnly(true);
    //  set the model that will be attached to
    ui->model->setText(*model);
    ui->project->setReadOnly(true);

    //  clear the source information
    ui->source->clear();

    //  initialize the dialog interface
    ui->useNode->setChecked(true);  // modeling type
    ui->useNew->setChecked(true);   // source type

    //  show the MODEL dialog
    exec();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  createModel: create a new model  */
void Set::createSetElem() {
    //  disable the edit mode
    editmode = false;

    //  set the model name
    ui->name->setText(proNew->name);
    //  set the project that will be attached to
    ui->project->setText(*project);
    ui->project->setReadOnly(true);

    //  set the model that will be attached to
    ui->model->setText(*model);
    ui->project->setReadOnly(true);

    //  clear the source information
    ui->source->clear();

    //  initialize the dialog interface
    ui->useElem->setChecked(true);  // modeling type
    ui->useNew->setChecked(true);   // source type

    //  show the MODEL dialog
    exec();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  saveModelDialog: save the dialog information to the variables  */
void Set::saveModelDialog() {
    //  assign the temporary model property
    proTem = editmode ? proOld : proNew;

    //  name of model
    proTem->name = ui->name->text();

    //  node type
    if (ui->useNode->isChecked()) proTem->type = 0;
    if (ui->useElem->isChecked()) proTem->type = 1;

    //  the name of the source
    proTem->source = ui->source->text();

    //  creation type
    if (ui->useNew->isChecked()) proTem->createType = 0;
    if (ui->useLocal->isChecked()) proTem->createType = 1;

    //  update the model name if in edit mode
    if (editmode) {
        //  update the name of the item
        item->setText(proTem->name);
    }
    //  add the created model to the list view
    else {
        //  create the item
        item = new QStandardItem(proTem->name);
        item->setData(QVariant::fromValue(proTem), Qt::UserRole);
        itemModel->appendRow(item);
        //  assign the item to null and create the new property object
        item   = nullptr;
        proNew = nullptr;
        ++index;
        proNew = new Property(index, project, model, workDir);
    }
    //  assign the temporary ModelProperty object to null
    proTem = nullptr;
    //  show the mananger dialog if possible
    if (isActiveMng) mng->show();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  confirmDialog: check the dialog information is complete  */
bool Set::confimDialog() {
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
void Set::loadModelDialog(int idx) {
    //  assign the edit mode
    editmode = true;
    //  get the current property
    item   = itemModel->item(idx, 0);
    proOld = item->data(Qt::UserRole).value<Property*>();
    //  set the name of the model
    ui->name->setText(proOld->name);
    ui->project->setText(*project);
    ui->project->setReadOnly(true);
    ui->model->setText(*model);
    ui->model->setReadOnly(true);

    //  set the model type
    switch (proOld->type) {
        case 0:
            ui->useNode->setChecked(true);
            break;
        case 1:
            ui->useElem->setChecked(true);
            break;
    }

    //  set the source
    ui->source->setText(proOld->source);

    //  set the create type
    switch (proOld->createType) {
        case 0:
            ui->useNew->setChecked(true);
            break;
        case 1:
            ui->useLocal->setChecked(true);
            break;
    }
    //  show the model dialog
    exec();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  getModelName: get the model name and assign ti to the Rename dialog  */
void Set::getModelName(int idx) {
    //  set the old name
    if (idx >= 0) {
        //  get the item
        item = itemModel->item(idx, 0);
        //  get the current model property in edit
        proOld = item->data(Qt::UserRole).value<Property*>();
        //  assign the old name
        rename->setNameOriginal(proOld->name);
        //  show the rename dialog
        rename->exec();
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  getNewName: get the new name of the material  */
void Set::setModelName() {
    //  set the new name of the model
    QString text;
    rename->getNameNew(text);
    proOld->name = text;
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
void Set::deleteModel(int idx) {
    //  check the selection is valide or not
    if (idx >= 0) {
        //  get the item
        item = itemModel->item(idx, 0);
        //  get the current model property in edit
        proOld = item->data(Qt::UserRole).value<Property*>();
        //  remove the item from the list view
        itemModel->removeRow(idx);
        //  remove the property object
        delete proOld;
        proOld = nullptr;
    }
}

/*  ############################################################################
 *  saveModelDatabase: save the basic information of the model database*/
void Set::saveModelDatabase() {
    //  iterate through all items in the list view
    for (int i = 0; i < itemModel->rowCount(); ++i) {
        //  get the item
        item = itemModel->item(i, 0);
        //  get the current model property
        proOld = item->data(Qt::UserRole).value<Property*>();
        //  save the database
        proOld->saveDatabase(*workDir);
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  loadModel: load the model information from the local database  */
void Set::loadModelDatabase() {
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
    proNew->name = query.value(2).toString();
    qDebug() << proNew->name;

    //  get the model type
    query.next();
    proNew->type = query.value(2).toInt();
    qDebug() << proNew->type;

    //  get the source
    query.next();
    proNew->source = query.value(2).toString();
    qDebug() << proNew->source;

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

Set::Property::Property(int index, QString*& projName, QString*& modelName,
                        QString*& path) {
    //  initialize the flag information
    assigned   = false;
    ifExist    = false;
    ifOverride = false;
    ifCreateDb = false;

    //  assign the model name
    name = "Set-" + QString::number(index);
}

Set::Property::~Property() {
    project = nullptr;
    model   = nullptr;
    workDir = nullptr;
}

/*  ############################################################################
 *  saveDatabase: save model information to the local database */
void Set::Property::saveDatabase(QString& workDir) {
    //  create the database
    if (!ifCreateDb) {
        db = QSqlDatabase::addDatabase("QSQLITE", name);
    }
}
