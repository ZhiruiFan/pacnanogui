/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Material.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 14th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "material.h"

#include "ui_material.h"

/*  ############################################################################
 *  constructor:
    create the mateiral creation dialog  */
Material::Material(QWidget* parent) : QDialog(parent), ui(new Ui::Material) {
    //  setup the UI
    ui->setupUi(this);

    //  Define style sheet
    btnstyle = "QPushButton::menu-indicator { image: none; }";

    //  setup the Item configuration
    actDen     = new QAction("Density");
    actLinear  = new QAction("Linear Material");
    actNeo     = new QAction("Neo-Hookean Material");
    actExpan   = new QAction("Thermal Expansion");
    actConduct = new QAction("Thermal Conduction");

    //  setup validator
    validator = new QDoubleValidator(this);

    //  create the new material property
    index  = 0;
    proNew = new Property(index);

    //  create listview model
    setupMatProListview();

    //  setup the material properties penals
    setupGeneralPro();     // general material properties
    setupMechanicalPro();  // mechanical properties
    setupThermoPro();      // thermal properties
    setupElecMag();        // electromagnetic
    setupOthers();         // others
    setupManager();        // setup the manager
    setupRename();         // setup the rename dialog
    //  set an empty penal
    ui->matProperty->setCurrentIndex(0);

    //  button cancel
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->btnOk, &QPushButton::clicked, this, &QDialog::accept);

    //  button discard
    connect(ui->btnDenDiscard, &QPushButton::clicked, this, &Material::discard);
    connect(ui->btnLinearDiscard, &QPushButton::clicked, this,
            &Material::discard);
    connect(ui->btnNeoDiscard, &QPushButton::clicked, this, &Material::discard);
    connect(ui->btnThermalDiscard, &QPushButton::clicked, this,
            &Material::discard);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor: destroy the material object  */
Material::~Material() {
    //  assign empty the the material property
    proNew = nullptr;
    proOld = nullptr;
    proCur = nullptr;

    //  delete the vables
    delete ui;
    delete general;
    delete mechanical;
    delete thermal;
    delete elecMag;
    delete other;
    delete actDen;
    delete actLinear;
    delete actNeo;
    delete actExpan;
    delete actConduct;
    delete selectModel;
    delete itemPro;
}

/*  ############################################################################
 *  setupMatProListview:
 *      create the listview model for the created material properties  */
void Material::setupMatProListview() {
    //  create and assign the item model
    itemPro = new QStandardItemModel(this);
    //  create the material listview
    ui->matProList->setModel(itemPro);
    selectModel = ui->matProList->selectionModel();

    // set single selection for listview
    ui->matProList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->matProList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //  link to delete button
    connect(selectModel, &QItemSelectionModel::selectionChanged, this,
            [&](const QItemSelection& selected) {
                ui->btnDelete->setEnabled(!selected.isEmpty());
            });
    connect(ui->btnDelete, &QToolButton::clicked, this,
            &Material::delSelectedMatProItem);
    ui->btnDelete->setEnabled(false);
    connect(ui->matProList, &QListView::clicked, this, [&]() {
        //  get the current index in the property list view
        QModelIndex index = selectModel->currentIndex();

        //  switch the pages in the list view
        switch (map[index.data().toString()]) {
            case 0:
                ui->matProperty->setCurrentIndex(1);
                break;
            case 1:
                ui->matProperty->setCurrentIndex(2);
                break;
            case 2:
                ui->matProperty->setCurrentIndex(3);
                break;
            case 3:
                ui->matProperty->setCurrentIndex(4);
                break;
            case 4:
                ui->matProperty->setCurrentIndex(4);
                break;
        }
    });
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Genearal material properties  */
void Material::setupGeneralPro() {
    //  create general menu
    general = new QMenu(this);

    //  create and link density
    general->addAction(actDen);
    ui->btnGeneral->setMenu(general);
    connect(actDen, &QAction::triggered, this,
            [&]() { ui->matProperty->setCurrentIndex(1); });
    ui->btnGeneral->setStyleSheet(btnstyle);
    ui->dataDen->setValidator(validator);

    //  set default mass matrix type
    ui->massUniform->setChecked(true);

    //  Apply button
    connect(ui->btnDenApply, &QPushButton::clicked, this,
            &Material::addItemDen);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Mechanical material properties  */
void Material::setupMechanicalPro() {
    //  create menu
    mechanical = new QMenu(this);

    //  linear properties
    mechanical->addAction(actLinear);
    ui->selectLienarElastic->setChecked(true);
    ui->dataLinearModulus->setValidator(validator);
    ui->dataLinearPoison->setValidator(validator);
    connect(actLinear, &QAction::triggered, this,
            [&]() { ui->matProperty->setCurrentIndex(2); });
    connect(ui->selectLienarElastic, &QRadioButton::clicked, this,
            &Material::setLinearElastic);
    connect(ui->selectlienarBulk, &QRadioButton::clicked, this,
            &Material::setLinearBulk);

    //  neo-Hookean properties
    mechanical->addAction(actNeo);
    ui->selectNeoElastic->setChecked(true);
    ui->dataNeoModulus->setValidator(validator);
    ui->dataNeoPoison->setValidator(validator);
    connect(actNeo, &QAction::triggered, this,
            [&]() { ui->matProperty->setCurrentIndex(3); });
    connect(ui->selectNeoElastic, &QRadioButton::clicked, this,
            &Material::setNeoElastic);
    connect(ui->selectNeoBulk, &QRadioButton::clicked, this,
            &Material::setNeoBulk);

    //  set button style
    ui->btnMechanical->setMenu(mechanical);
    ui->btnMechanical->setStyleSheet(btnstyle);

    //  Apply button
    connect(ui->linearApply, &QPushButton::clicked, this,
            &Material::addItemLinear);
    connect(ui->neoApply, &QPushButton::clicked, this, &Material::addItemNeo);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Thermal material properties  */
void Material::setupThermoPro() {
    //  create thermal property menu
    thermal = new QMenu(this);

    //  thermal conduction
    thermal->addAction(actConduct);
    connect(actConduct, &QAction::triggered, this, [&]() {
        //  switch penal
        ui->matProperty->setCurrentIndex(4);
        //  set checked
        ui->selectConduct->setChecked(true);
        ui->dataConduct->setEnabled(true);
        ui->selectExpan->setChecked(false);
        ui->dataExpan->setEnabled(false);
    });
    connect(ui->selectConduct, &QCheckBox::stateChanged, this,
            &Material::setConduction);

    //  thermal expansion
    thermal->addAction(actExpan);
    ui->btnThermal->setMenu(thermal);
    connect(actExpan, &QAction::triggered, this, [&]() {
        //  switch penal
        ui->matProperty->setCurrentIndex(4);
        //  set checked
        ui->selectExpan->setChecked(true);
        ui->dataExpan->setEnabled(true);
        ui->selectConduct->setChecked(false);
        ui->dataConduct->setEnabled(false);
    });
    connect(ui->selectExpan, &QCheckBox::stateChanged, this,
            &Material::setExpansion);

    //  button style
    ui->btnThermal->setStyleSheet(btnstyle);

    //  Apply button
    connect(ui->thermalApply, &QPushButton::clicked, this,
            &Material::addItemThermo);
}
/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setup the Electrical and Magnical material properties  */
void Material::setupElecMag() {
    //  create the manu of electicity and magnity
    elecMag = new QMenu("ElecMag");
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setup the other added properties  */
void Material::setupOthers() {
    //  create menu of other properties
    other = new QMenu("Other");
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setup material manager object  */
void Material::setupManager() {
    //  create the listview related model
    itemModel = new QStandardItemModel(this);
    //  create the manager
    mng = new Manager(this, "Material Manager", ":/icons/material.png",
                      itemModel);
    //  assign the active flag
    isActiveMng = false;

    //  connect the signal and button
    connect(mng, &Manager::signalCreate, this, &Material::create);
    connect(mng, &Manager::signalEdit, this, &Material::edit);
    connect(mng, &Manager::signalRename, this, &Material::setOldName);
    connect(mng, &Manager::signalDelete, this, &Material::remove);
    connect(mng, &Manager::shown, this, [&]() { isActiveMng = true; });
    connect(mng, &Manager::closed, this, [&]() { isActiveMng = false; });
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setup Rename dialog  */
void Material::setupRename() {
    //  rename dialog
    rename = new Rename(this);
    //  connect OK button of the dialog
    connect(rename, &QDialog::accepted, this, &Material::getNewName);
}

/*  ############################################################################
 *  create: create the material object  */
void Material::create() {
    //  disable the edit mode
    editmode = false;

    //  assign the material name
    ui->matName->setText(proNew->name);

    //  set the the delete button disabled
    ui->btnDelete->setDisabled(true);

    //  show the material creation dialog
    exec();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  addItemDen: add the density item for the material properties list view*/
void Material::addItemDen() {
    //  determine current edit property
    proCur = editmode ? proOld : proNew;
    //  add the density item
    if (!(proCur->den->assigned)) {
        if (ui->dataDen->text().isEmpty()) {
            msgbox.setWindowTitle("Density");
            msgbox.setText("Please input the material density.");
            msgbox.setIcon(QMessageBox::Critical);
            msgbox.setWindowIcon(QIcon(":/icons/pacnano.png"));
            msgbox.show();
        } else {
            itemPro->appendRow(new QStandardItem("Density"));
            proCur->den->updateAssign(true);
        }
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  add linear material item in listview  */
void Material::addItemLinear() {
    //  determine current edit property
    proCur = editmode ? proOld : proNew;
    //  add the linear property item
    if (!(proCur->linear->assigned)) {
        if (ui->dataLinearModulus->text().isEmpty() ||
            ui->dataLinearPoison->text().isEmpty()) {
            msgbox.setWindowTitle("Linear Material");
            msgbox.setText("Please complete the material properties.");
            msgbox.setIcon(QMessageBox::Critical);
            msgbox.setWindowIcon(QIcon(":/icons/pacnano.png"));
            msgbox.show();
        } else {
            itemPro->appendRow(new QStandardItem("Linear Material"));
            proCur->linear->updateAssign(true);
        }
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  add Neo-Hookean material item to the listview  */
void Material::addItemNeo() {
    //  determine current edit property
    proCur = editmode ? proOld : proNew;
    //  add the neo-Hookean property item
    if (!(proCur->neo->assigned)) {
        if (ui->dataNeoModulus->text().isEmpty() ||
            ui->dataNeoPoison->text().isEmpty()) {
            msgbox.setWindowTitle("Neo-Hookean Material");
            msgbox.setText("Please complete the material properties.");
            msgbox.setIcon(QMessageBox::Critical);
            msgbox.setWindowIcon(QIcon(":/icons/pacnano.png"));
            msgbox.show();
        } else {
            itemPro->appendRow(new QStandardItem("Neo-Hookean Material"));
            proCur->neo->updateAssign(true);
        }
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  add thermal property items to the listview  */
void Material::addItemThermo() {
    //  determine current edit property
    proCur = editmode ? proOld : proNew;
    //  thermal conduction
    if (ui->selectConduct->isChecked()) {
        if (!(proCur->conduct->assigned)) {
            if (ui->dataConduct->text().isEmpty()) {
                msgbox.setWindowTitle("Thermal Conduction");
                msgbox.setText("Please input the thermal conduction.");
                msgbox.setIcon(QMessageBox::Critical);
                msgbox.setWindowIcon(QIcon(":/icons/pacnano.png"));
                msgbox.show();
            } else {
                itemPro->appendRow(new QStandardItem("Thermal Conduction"));
                proCur->conduct->updateAssign(true);
            }
        }
    }

    //  thermal expansion
    if (ui->selectExpan->isChecked()) {
        if (!(proCur->expan->assigned)) {
            if (ui->dataExpan->text().isEmpty()) {
                msgbox.setWindowTitle("Thermal Expansion");
                msgbox.setText("Please input the thermal expansion.");
                msgbox.setIcon(QMessageBox::Critical);
                msgbox.setWindowIcon(QIcon(":/icons/pacnano.png"));
                msgbox.show();
            } else {
                itemPro->appendRow(new QStandardItem("Thermal Expansion"));
                proCur->expan->updateAssign(true);
            }
        }
    }
}

/*  ############################################################################
 *  edit the information of selected material  */
void Material::edit(int idx) {
    //  assign the edit mode
    editmode = true;
    //  get the current property
    item   = itemModel->item(idx, 0);
    proOld = item->data(Qt::UserRole).value<Property*>();

    //  clean the items
    itemPro->clear();
    selectModel->clear();

    //  load the name of material
    ui->matName->setText(proOld->name);

    //  recory material density
    if (proOld->den->assigned) {
        itemModel->appendRow(new QStandardItem("Density"));
        ui->dataDen->setText(QString::number(proOld->den->data[0]));
        if (proOld->den->flag[0] == 0) {
            ui->massUniform->setChecked(true);
        } else {
            ui->massConcentrated->setChecked(true);
        }
    }

    //  linear material
    if (proOld->linear->assigned) {
        itemModel->appendRow(new QStandardItem("Linear Material"));
        ui->dataLinearModulus->setText(
            QString::number(proOld->linear->data[0]));
        ui->dataLinearPoison->setText(QString::number(proOld->linear->data[1]));
        if (proOld->linear->flag[0] == 0) {
            ui->selectLienarElastic->setChecked(true);
        } else {
            ui->selectlienarBulk->setChecked(true);
        }
    }

    //  neo-Hookean material
    if (proOld->neo->assigned) {
        itemModel->appendRow(new QStandardItem("Neo-Hookean Material"));
        proOld->neo->data[0] = ui->dataNeoModulus->text().toDouble();
        proOld->neo->data[1] = ui->dataNeoPoison->text().toDouble();
        if (ui->selectNeoElastic->isChecked()) {
            proOld->neo->flag[0] = 0;
        } else {
            proOld->neo->flag[0] = 0;
        }
    }

    //  thermal conduction
    if (proOld->conduct->assigned) {
        itemModel->appendRow(new QStandardItem("Thermal Conduction"));
        if (ui->selectConduct->isChecked()) {
            proOld->conduct->data[0] = ui->dataConduct->text().toDouble();
            proOld->conduct->flag[0] = 1;
        } else {
            proOld->conduct->flag[0] = 0;
        }
    }

    //  thermal expansion
    if (proOld->expan->assigned) {
        itemModel->appendRow(new QStandardItem("Thermal Expansion"));
        if (ui->selectExpan->isChecked()) {
            proOld->expan->data[0] = ui->dataExpan->text().toDouble();
            proOld->expan->flag[0] = 1;
        } else {
            proOld->expan->flag[0] = 0;
        }
    }

    //  reset the current index
    ui->matProperty->setCurrentIndex(0);
}

/*  ############################################################################
 *  downloadProperty: when the OK button is pressed, the material property will
 *  assigned to the material property object  */
void Material::saveMaterial() {
    //  assign the temporary model property
    proCur = editmode ? proOld : proNew;

    //  material name
    proCur->name    = ui->matName->text();
    proCur->descrip = ui->matDiscrip->toPlainText();

    //  material density
    proCur->den->data[0] = ui->dataDen->text().toDouble();
    if (ui->massUniform->isChecked()) {
        proCur->den->flag[0] = 0;
    } else {
        proCur->den->flag[0] = 1;
    }

    //  linear material
    proCur->linear->data[0] = ui->dataLinearModulus->text().toDouble();
    proCur->linear->data[1] = ui->dataLinearPoison->text().toDouble();
    if (ui->selectLienarElastic->isChecked()) {
        proCur->linear->flag[0] = 0;
    } else {
        proCur->linear->flag[0] = 0;
    }

    //  neo-Hookean material
    proCur->neo->data[0] = ui->dataNeoModulus->text().toDouble();
    proCur->neo->data[1] = ui->dataNeoPoison->text().toDouble();
    if (ui->selectNeoElastic->isChecked()) {
        proCur->neo->flag[0] = 0;
    } else {
        proCur->neo->flag[0] = 0;
    }

    //  thermal conduction
    if (ui->selectConduct->isChecked()) {
        proCur->conduct->data[0] = ui->dataConduct->text().toDouble();
        proCur->conduct->flag[0] = 1;
    } else {
        proCur->conduct->flag[0] = 0;
    }

    //  thermal expansion
    if (ui->selectExpan->isChecked()) {
        proCur->expan->data[0] = ui->dataExpan->text().toDouble();
        proCur->expan->flag[0] = 1;
    } else {
        proCur->expan->flag[0] = 0;
    }

    //  update the model name if in edit mode
    if (editmode) {
        //  update the name of the item
        item->setText(proCur->name);
    }
    //  add the created model to the list view
    else {
        //  create the item
        item = new QStandardItem(proCur->name);
        item->setData(QVariant::fromValue(proCur), Qt::UserRole);
        itemModel->appendRow(item);
        //  assign the item to null and create the new property object
        item   = nullptr;
        proNew = nullptr;
        ++index;
        proNew = new Property(index);
    }
    //  assign the temporary ModelProperty object to null
    proCur = nullptr;
    //  show the mananger dialog if possible
    if (isActiveMng) mng->show();

    //  reset the dialog
    reset();
    ui->matProperty->setCurrentIndex(0);
}

/*  ############################################################################
 *  remove: remove the selected material property  */
void Material::remove(int idx) {
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

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  delMatProItem: delete the selected material item  */
void Material::delSelectedMatProItem() {
    //  assign the temporary model property
    proCur = editmode ? proOld : proNew;
    //  get the item
    item = itemPro->itemFromIndex(selectModel->currentIndex());
    //  get the name of the item
    QString text = item->text();
    // remove the item
    itemPro->removeRow(item->row());
    //  update the property
    switch (map[text]) {
        case 0:
            proCur->den->updateAssign(false);
            resetDensity();
            break;
        case 1:
            proCur->linear->updateAssign(false);
            resetLinear();
            break;
        case 2:
            proCur->neo->updateAssign(false);
            resetNeo();
            break;
        case 3:
            proCur->expan->updateAssign(false);
            resetExpan();
            break;
        case 4:
            proCur->conduct->updateAssign(false);
            resetConduct();
    }
}

/*  ############################################################################
 *  setOldName: set the old name to the Rename dialog  */
void Material::setOldName(int idx) {
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
void Material::getNewName() {
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
 *  reset: reset the dialog to the default  */
void Material::reset() {
    //  define the temporary index
    QModelIndex index;
    //  get the items in the current list view
    for (int i = 0; i < itemPro->rowCount(); ++i) {
        //  get the index
        index = itemPro->index(i, 0);

        //  remote the item
        itemPro->takeItem(index.row());
        itemPro->removeRow(index.row());

        //  reset the data
        switch (map[index.data().toString()]) {
            case 0:
                resetDensity();
                break;
            case 1:
                resetLinear();
                break;
            case 2:
                resetNeo();
                break;
            case 3:
                resetExpan();
                break;
            case 4:
                resetConduct();
                break;
        }
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  discard: discard the current material page  */
void Material::discard() {
    //  reset the current page
    switch (ui->matProperty->currentIndex()) {
        case 1:
            resetDensity();
            break;
        case 2:
            resetLinear();
            break;
        case 3:
            resetNeo();
            break;
        case 4:
            resetExpan();
            resetConduct();
            break;
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  reset density property  */
void Material::resetDensity() {
    ui->dataDen->clear();
    ui->massUniform->setChecked(true);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  reset Linear material  */
void Material::resetLinear() {
    ui->dataLinearModulus->clear();
    ui->dataLinearPoison->clear();
    ui->selectLienarElastic->setChecked(true);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  reset Neo-Hookean material  */
void Material::resetNeo() {
    ui->dataNeoModulus->clear();
    ui->dataNeoPoison->clear();
    ui->selectNeoElastic->setChecked(true);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  reset thermal conduction  */
void Material::resetConduct() {
    ui->dataConduct->clear();
    ui->selectConduct->setChecked(false);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  reset thermal expansion  */
void Material::resetExpan() {
    ui->dataExpan->clear();
    ui->selectExpan->setChecked(false);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  switch between elastic and bulk  */
void Material::setLinearBulk() { ui->linearLabel->setText("Bulk Modulus"); }
void Material::setNeoElastic() { ui->neoLabel->setText("Elastic Modulus"); }
void Material::setNeoBulk() { ui->neoLabel->setText("Bulk Modulus"); }
void Material::setLinearElastic() {
    ui->linearLabel->setText("Elastic Modulus");
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  set status of thermal conduction and expansion  */
void Material::setExpansion(int flag) {
    if (flag == Qt::Checked) {
        ui->dataExpan->setEnabled(true);
        ui->dataExpan->setValidator(validator);
    } else {
        ui->dataExpan->setEnabled(false);
    }
}

void Material::setConduction(int flag) {
    if (flag == Qt::Checked) {
        ui->dataConduct->setEnabled(true);
        ui->dataConduct->setValidator(validator);
    } else {
        ui->dataConduct->setEnabled(false);
    }
}

/*  ############################################################################
 *  manager: show the manager of material  */
void Material::manager() {
    //  show the material mananger
    mng->show();
    //  assign the material mananger shown flag
    isActiveMng = true;
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  closeEvent: override the close event of material creation  */
void Material::closeEvent(QCloseEvent* event) {
    //  if the mananger is active,then show the manager dialog
    if (isActiveMng) mng->show();
    //  perform the default close event
    QDialog::closeEvent(event);
}
/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  showEvent: override the show event of the material creation  */
void Material::showEvent(QShowEvent* event) {
    //  if manager is active, then hide the mananger dialog
    if (isActiveMng) mng->hide();
    //  perform the default show event
    QDialog::showEvent(event);
}

/*  ############################################################################
 *  constructor:  create the material property object
    @argc  _index: the inner order of the current material  */
Material::Property::Property(const int& _index) {
    //  assign the index
    index = _index;

    //  if material assigned
    assigned = false;

    //  initialize the material name
    name = QString("Material_") + QString::number(index);

    //  create the configuration for each material item
    den     = new Config("Density", 1, 1);
    linear  = new Config("Linear Material", 2, 1);
    neo     = new Config("Neo-Hookean Material", 2, 1);
    expan   = new Config("Thermal Expansion", 1, 1);
    conduct = new Config("Thermal Conduction", 1, 1);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor: destroy the material property object  */
Material::Property::~Property() {
    //  delete vriables
    delete den;
    delete linear;
    delete neo;
    delete expan;
    delete conduct;

    //  assign NULL
    den     = NULL;
    linear  = NULL;
    neo     = NULL;
    expan   = NULL;
    conduct = NULL;
}

/*  ############################################################################
 *  Config: nested class for the detailed configuration of each material
 *  properties
    @argc  _name: name of the configuration
    @argc  _numData: number of data information
    @argc  _numFlag: number of configuration flag  */
Material::Property::Config::Config(const QString& _name, const int& _numData,
                                   const int& _numFlag) {
    //  initialize the data information
    data = new double[_numData];
    //  initialize the flags
    flag = new int[_numFlag];
    //  assign status
    assigned = false;
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor: destroy the config object  */
Material::Property::Config::~Config() {
    //  delete variables
    delete[] data;
    delete[] flag;

    //  assign NULL
    data = NULL;
    flag = NULL;
}
