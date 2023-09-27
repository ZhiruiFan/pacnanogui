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

#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "ui_remote.h"

Remote::Remote(QWidget *parent) : QDialog(parent), ui(new Ui::Remote) {
    /*  CREATE UI  */
    ui->setupUi(this);

    /*  CONNECTIONS  */
    //  calcel button
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);

    //  set the data format of ip address
    QRegularExpression ipRegExp(
        "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator ipValidator(ipRegExp, ui->ipAddr);
    ui->ipAddr->setValidator(&ipValidator);

    //  set the format of password
    ui->passWd->setEchoMode(QLineEdit::Password);
    ui->userName->setEchoMode(QLineEdit::Normal);
}

Remote::~Remote() { delete ui; }
