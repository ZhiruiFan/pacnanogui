/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : rename.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 16th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "rename.h"

#include "ui_rename.h"

/*  ############################################################################
 *  constructor: create a rename object  */
Rename::Rename(QWidget *parent) : QDialog(parent), ui(new Ui::Rename) {
    //  create the UI interface
    ui->setupUi(this);

    //  set orginal name lineedit unediable
    ui->nameOrig->setDisabled(true);

    //  connect the cancel button
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
    //  connect the OK button
    connect(ui->btnOk, &QPushButton::clicked, this, &QDialog::accept);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  setNameOriginal: set the original name  */
void Rename::setNameOriginal(QString &_nameOrig) {
    //  clear the new name
    ui->nameNew->clear();
    //  set the original name
    ui->nameOrig->setText(_nameOrig);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  getNameNew: get the new name of object  */
void Rename::getNameNew(QString &name) { name = ui->nameNew->text(); }

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor: destroy the rename object  */
Rename::~Rename() { delete ui; }
