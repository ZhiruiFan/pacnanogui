/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Post.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : November 22th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "post.h"

#include "ui_post.h"

/*  ############################################################################
 *  constructor: create the Post object  */
Post::Post(QWidget *parent) : QDialog(parent), ui(new Ui::Post) {
    /*  setup the UI interface  */
    ui->setupUi(this);
    reset();

    /*  connect to the CANCEL button  */
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);

    /*  connect to the OK button  */
    connect(ui->btnOk, &QPushButton::clicked, this, [&]() {
        //  warp scale
        if (ui->useUndeform->isChecked())
            warpScale = 0.0;
        else
            warpScale = ui->warpScale->value();

        //  limit value
        if (ui->useLimit->isChecked()) {
            // ## USE BOTH ##
            if (ui->useLowerLimit->isChecked() &&
                ui->useUpperLimit->isChecked()) {
                thresholdFlag = 1;
                lowerLimit    = ui->lowerLimit->value();
                upperLimit    = ui->upperLimit->value();
            }
            // ## USE LOWER ONLY ##
            else if (ui->useLowerLimit->isChecked()) {
                thresholdFlag = 2;
                lowerLimit    = ui->lowerLimit->value();
                upperLimit    = 1.0;
            }
            // ## USE UPPER ONLY ##
            else {
                thresholdFlag = 3;
                lowerLimit    = 0.0;
                upperLimit    = ui->upperLimit->value();
            }
        } else {
            thresholdFlag = 0;
            lowerLimit    = 0.0;
            upperLimit    = 1.0;
        }

        //  legend configuration
        numIntervals = ui->numIntervals->value();
        isAutoLegend = ui->useAutoRange->isChecked() ? true : false;

        //  active the accept signal
        accept();
    });

    /*  connect to the RESET button  */
    connect(ui->btnReset, &QPushButton::clicked, this, &Post::reset);

    /*  connect the status of warp configuration  */
    connect(ui->useDeform, &QRadioButton::clicked, this,
            [&]() { ui->warpScale->setDisabled(false); });
    connect(ui->useUndeform, &QRadioButton::clicked, this,
            [&]() { ui->warpScale->setDisabled(true); });

    /*  connect to the status of threshold  */
    connect(ui->useLimit, &QRadioButton::clicked, this, [&]() {
        ui->useLowerLimit->setDisabled(false);
        ui->lowerLimit->setDisabled(false);
        ui->useUpperLimit->setDisabled(false);
        ui->upperLimit->setDisabled(false);
    });
    connect(ui->useNoLimit, &QRadioButton::clicked, this, [&]() {
        ui->useLowerLimit->setDisabled(true);
        ui->lowerLimit->setDisabled(true);
        ui->useUpperLimit->setDisabled(true);
        ui->upperLimit->setDisabled(true);
    });
}

/*  ============================================================================
 *  destructor: destroy the Post object  */
Post::~Post() { delete ui; }

/*  ############################################################################
 *  reset: reset the diag to the original  */
void Post::reset() {
    /*  Warping configuration  */
    warpScale = 0.0;
    ui->useUndeform->setChecked(true);
    ui->useDeform->setChecked(false);
    ui->warpScale->setDisabled(true);
    ui->warpScale->setValue(warpScale);

    /*  Threshold switch  */
    thresholdFlag = 0;
    ui->useNoLimit->setChecked(true);

    /*  Lower limit  */
    lowerLimit = 0.0;
    ui->useLowerLimit->setChecked(false);
    ui->useLowerLimit->setDisabled(true);
    ui->lowerLimit->setDisabled(true);
    ui->lowerLimit->setValue(lowerLimit);

    /*  Upper limit  */
    upperLimit = 1.0;
    ui->useUpperLimit->setChecked(false);
    ui->useUpperLimit->setDisabled(true);
    ui->upperLimit->setDisabled(true);
    ui->upperLimit->setValue(upperLimit);

    /*  Legend configuration  */
    ui->numIntervals->setValue(12);
    ui->useFixRange->setChecked(true);
    ui->useAutoRange->setChecked(false);
}

/*  ############################################################################
 *  getWarpScale: get the warp scale coefficient
 *  @return  the warping scale coefficient  */
double Post::getWarpScale() { return warpScale; }

/*  ============================================================================
 *  getLimitType: get the type of threshold. If 0, then no limit is used;
 *  if 1, the upper and lower limit are both used; if 2, then only lower
 *  limit is used; if 3, then only the upper limit is used
 *  @return  the limit type  */
int Post::getLimitType() { return thresholdFlag; }

/*  ============================================================================
 *  getLowerLimit: get the value of the lower limit
 *  @return  lower limit value  */
double Post::getLowerLimit() { return lowerLimit; }

/*  ============================================================================
 *  getUpperLimit: get the value of the upper limit
 *  @return  upper limit value  */
double Post::getUpperLimit() { return upperLimit; }

/*  getNumIntervals: get the number of intervals in the legend
 *  @return  the number of the intervals   */
int Post::getNumIntervals() { return numIntervals; }

/*  isUseAutoLegend: is the legend range is automatic or fixed
 *  @return   get the status of the legend range  */
bool Post::isUseAutoLegend() { return isAutoLegend; }
