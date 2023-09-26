/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Manager.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 19th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "manager.h"

#include "ui_manager.h"

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  constructor: create a Manager object  */
Manager::Manager(QWidget *parent, const QString &winTitle, const QString &icon,
                 QStandardItemModel *&_itemModel)
    : QDialog(parent), ui(new Ui::Manager) {
    //  create the UI interface
    ui->setupUi(this);

    //  assign the item model to the list view
    itemModel = _itemModel;
    ui->listView->setModel(itemModel);
    selectModel = ui->listView->selectionModel();
    connect(selectModel, &QItemSelectionModel::selectionChanged, this,
            [&](const QItemSelection &selected) {
                ui->btnEdit->setEnabled(!selected.isEmpty());
                ui->btnRename->setEnabled(!selected.isEmpty());
                ui->btnDelete->setEnabled(!selected.isEmpty());
            });

    //  set the single selection mode
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //  set the window title and icons
    setWindowTitle(winTitle);
    setWindowIcon(QIcon(icon));

    //  set the initial status of the button
    ui->btnEdit->setEnabled(false);
    ui->btnRename->setEnabled(false);
    ui->btnDelete->setEnabled(false);

    //  connect between the button and signla
    connect(ui->btnCreate, &QPushButton::clicked,  //
            this, &Manager::signalCreate);
    connect(ui->btnEdit, &QPushButton::clicked,    //
            this, &Manager::onClickEdit);
    connect(ui->btnDelete, &QPushButton::clicked,  //
            this, &Manager::onClickDelete);
    connect(ui->btnRename, &QPushButton::clicked,  //
            this, &Manager::onClickRename);
    connect(ui->btnClose, &QPushButton::clicked,   //
            this, &QDialog::close);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor: destroy the Manager object  */
Manager::~Manager() {
    delete ui;
    itemModel = nullptr;
}

/*  ############################################################################
 *  get the selected item in the list view  */
void Manager::getSelectedIndex() {
    //  get the current index
    index = selectModel->currentIndex();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  onClickEdit: handle signal of the edit button  */
void Manager::onClickEdit() {
    index = selectModel->currentIndex();
    emit signalEdit(index.row());
}
void Manager::onClickDelete() {
    index = selectModel->currentIndex();
    emit signalDelete(index.row());
}
void Manager::onClickRename() {
    index = selectModel->currentIndex();
    emit signalRename(index.row());
}
