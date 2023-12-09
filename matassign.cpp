/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : matassign.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : December 8th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "matassign.h"

#include "ui_matassign.h"

MatAssign::MatAssign(QWidget *parent) : QDialog(parent), ui(new Ui::MatAssign) {
    ui->setupUi(this);
}

MatAssign::~MatAssign() { delete ui; }
