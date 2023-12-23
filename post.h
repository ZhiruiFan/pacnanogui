/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Post.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : November 22th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef POST_H
#define POST_H

#include <QDialog>

namespace Ui {
class Post;
}

class Post : public QDialog {
    Q_OBJECT

private:
    Ui::Post *ui;               // UI interface

    double warpScale;           // warp scale

    int thresholdFlag;          // flag for threshold
    double lowerLimit;          // lower limit of threshold
    double upperLimit;          // upper limit of threshold

    double isDynamicScalarBar;  // style of the scalar
    int numIntervals;           // number of intervals

public:
    /*  ########################################################################
     *  constructor: create the Post object  */
    explicit Post(QWidget *parent = nullptr);

    /*  destructor: destroy the Post object  */
    ~Post();

public:
    /*  ########################################################################
     *  getWarpScale: get the warp scale coefficient
     *  @return  the warping scale coefficient  */
    double getWarpScale();

    /*  getLimitType: get the type of threshold. If 0, then no limit is used;
     *  if 1, the upper and lower limit are both used; if 2, then only lower
     *  limit is used; if 3, then only the upper limit is used
     *  @return  the limit type  */
    int getLimitType();

    /*  getLowerLimit: get the value of the lower limit
     *  @return  lower limit value  */
    double getLowerLimit();

    /*  getUpperLimit: get the value of the upper limit
     *  @return  upper limit value  */
    double getUpperLimit();

private slots:
    /*  ########################################################################
     *  reset: reset the diag to the original  */
    void reset();
};

#endif  // POST_H
