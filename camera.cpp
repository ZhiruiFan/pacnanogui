/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : camera.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : December 12th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "camera.h"

#include "ui_camera.h"

/*  ############################################################################
 *  constructor: create the Camera object  */
Camera::Camera(QWidget *parent) : QDialog(parent), ui(new Ui::Camera) {
    /*  setup the dialog interface  */
    ui->setupUi(this);

    /*  initialize the camera parameters  */
    moveInc   = 1.0;
    rotateInc = 15.0;
    zoomInc   = 1.0;

    /*  connect to the buttons  */
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->btnOk, &QPushButton::clicked, this, [&]() {
        /*  get the parameters from the dialog  */
        moveInc   = ui->moveInc->value();
        rotateInc = ui->rotateInc->value();
        zoomInc   = ui->zoomInc->value();
        /*  close the window  */
        accept();
    });
    connect(ui->btnReset, &QPushButton::clicked, this, [&]() {
        ui->moveInc->setValue(1.0);
        ui->rotateInc->setValue(15.0);
        ui->zoomInc->setValue(1.0);
    });
}

/*  ============================================================================
 *  destructor: deallocate the Camera object  */
Camera::~Camera() { delete ui; }
