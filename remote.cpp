/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : remote.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 12th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */

#include "remote.h"

#include "ui_remote.h"

Remote::Remote(QWidget *parent) : QDialog(parent), ui(new Ui::Remote) {
    /*  CREATE UI  */
    ui->setupUi(this);

    /*  CONNECTIONS  */
    //  calcel button
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
}

Remote::~Remote() { delete ui; }
