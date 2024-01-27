/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : reflect.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : October 22th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "reflect.h"

#include "ui_reflect.h"

/*  ############################################################################
 *  constructor: create the Reflect object  */
Reflect::Reflect(QWidget *parent) : QDialog(parent), ui(new Ui::Reflect) {
    /*  initialize the UI interface  */
    ui->setupUi(this);

    /*  reset the contents  */
    reset();

    /*  connect to the operatin switch  */
    connect(ui->userMirror, &QRadioButton::clicked, this,
            [&]() { ui->operateSwitch->setCurrentIndex(0); });
    connect(ui->useRec, &QRadioButton::clicked, this,
            [&]() { ui->operateSwitch->setCurrentIndex(1); });
    connect(ui->useCircular, &QRadioButton::clicked, this,
            [&]() { ui->operateSwitch->setCurrentIndex(2); });

    /*  connect to buttons  */
    connect(ui->btnReset, &QPushButton::clicked, this, &Reflect::reset);
    connect(ui->btnCancel, &QPushButton::clicked, this, &Reflect::close);
    connect(ui->btnOk, &QPushButton::clicked, this, &Reflect::save);

    /*  connect to the rectangular patterns  */
    connect(ui->useRecXY, &QCheckBox::stateChanged, this, [&](int state) {
        ui->recNumXY->setEnabled(state == Qt::Checked);
        ui->recOffsetXY->setEnabled(state == Qt::Checked);
    });
    connect(ui->useRecYZ, &QCheckBox::stateChanged, this, [&](int state) {
        ui->recNumYZ->setEnabled(state == Qt::Checked);
        ui->recOffsetYZ->setEnabled(state == Qt::Checked);
    });
    connect(ui->useRecXZ, &QCheckBox::stateChanged, this, [&](int state) {
        ui->recNumXZ->setEnabled(state == Qt::Checked);
        ui->recOffsetXZ->setEnabled(state == Qt::Checked);
    });
}

/*  ############################################################################
 *  destructor: destroy the Reflect object  */
Reflect::~Reflect() { delete ui; }

/*  ############################################################################
 *  reset: reset the information in the dialog  */
void Reflect::reset() {
    /*  type of operation  */
    ui->userMirror->setChecked(true);
    ui->useRec->setChecked(false);
    ui->useCircular->setChecked(false);

    /*  set the operatin switch  */
    ui->operateSwitch->setCurrentIndex(0);

    /*  set the mirror plate  */
    ui->useMirrorXY->setChecked(false);
    ui->useMirrorYZ->setChecked(false);
    ui->useMirrorXZ->setChecked(false);

    /*  set the rectangular plate (XY)  */
    ui->useRecXY->setChecked(false);
    ui->recNumXY->setValue(0);
    ui->recNumXY->setDisabled(true);
    ui->recOffsetXY->setValue(0);
    ui->recOffsetXY->setDisabled(true);

    /*  set the rectangular plate (YZ)  */
    ui->useRecYZ->setChecked(false);
    ui->recNumYZ->setValue(0);
    ui->recNumYZ->setDisabled(true);
    ui->recOffsetYZ->setValue(0);
    ui->recOffsetYZ->setDisabled(true);

    /*  set the rectangular plate (XZ)  */
    ui->useRecXZ->setChecked(false);
    ui->recNumXZ->setValue(0);
    ui->recNumXZ->setDisabled(true);
    ui->recOffsetXZ->setValue(0);
    ui->recOffsetXZ->setDisabled(true);

    /*  set the circular plate  */
    ui->useCircularX->setChecked(true);
    ui->useCircularY->setChecked(false);
    ui->useCircularZ->setChecked(false);
    ui->circularNumPattern->setValue(0);
    ui->circularTotalAngle->setValue(360.0);

    /* coordinates of original point  */
    ui->coordsX->setValue(0.0);
    ui->coordsY->setValue(0.0);
    ui->coordsZ->setValue(0.0);

    /*  rotation angle  */
    ui->rotationX->setValue(0.0);
    ui->rotationY->setValue(0.0);
    ui->rotationZ->setValue(0.0);

    /*  using full model or not  */
    ui->useModelAll->setChecked(false);
    ui->useModelCur->setChecked(true);
}

/*  ############################################################################
 *  save: save the dialog information to the local variables  */
void Reflect::save() {
    /*  type of operation  */
    if (ui->userMirror->isChecked()) type = 0;
    if (ui->useRec->isChecked()) type = 1;
    if (ui->useCircular->isChecked()) type = 2;

    /*  set the mirror information  */
    mirrorFlag[0] = ui->useMirrorXY->isChecked() ? true : false;
    mirrorFlag[1] = ui->useMirrorYZ->isChecked() ? true : false;
    mirrorFlag[2] = ui->useMirrorXZ->isChecked() ? true : false;

    /*  which rectangular pattern is used  */
    recPatternFlag[0] = ui->useRecXY->isChecked() ? true : false;
    recPatternFlag[1] = ui->useRecYZ->isChecked() ? true : false;
    recPatternFlag[2] = ui->useRecXZ->isChecked() ? true : false;

    /*  number of patterns  */
    numRecArraies[0] = ui->recNumXY->value();
    numRecArraies[1] = ui->recNumYZ->value();
    numRecArraies[2] = ui->recNumXZ->value();

    /*  offset in patterns  */
    offsets[0] = ui->recOffsetXY->value();
    offsets[1] = ui->recOffsetXZ->value();
    offsets[2] = ui->recOffsetYZ->value();

    /* coordinates of original point  */
    coords[0] = ui->coordsX->value();
    coords[1] = ui->coordsY->value();
    coords[2] = ui->coordsZ->value();

    /*  rotation angle  */
    angle[0]   = ui->rotationX->value();
    angle[1]   = ui->rotationY->value();
    angle[2]   = ui->rotationZ->value();
    totalAngle = ui->circularTotalAngle->value();

    /*  whether the full model is used  */
    isFullModel = ui->useModelAll->isChecked() ? true : false;

    /*  accept the dialog  */
    accept();
}
