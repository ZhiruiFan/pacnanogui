/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : reflect.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : J 22th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */

#ifndef REFLECT_H
#define REFLECT_H

#include <QDialog>

namespace Ui {
class Reflect;
}
/*  CLASS Reflect: perform mirroring or array patterns for the field variables.
 *  It is used for postprocessing the analysis or optimized results  */
class Reflect : public QDialog {
    Q_OBJECT

private:
    Ui::Reflect *ui;         // UI interface
    int type;                // the type of operation

    bool mirrorFlag[3];      // which plane is used for mirroring

    bool recPatternFlag[3];  // which plane is used for rectangular pattern
    int numRecArraies[3];    // number of arraies
    int offsets[3];          // offsets for between patterns

    bool cirPatternFlag[3];  // which axis is used for circular pattern
    int numCirArraies;       // number of circular arraies
    double totalAngle;       // total angles for circular pattern

    double coords[3];        // coordinates of original points
    double angle[3];         // rotation angle of from the original axis

    bool isFullModel;        // whether the full model is used

public:
    /*  constructor: create the Reflect object  */
    explicit Reflect(QWidget *parent = nullptr);
    /*  destructor: destroy the Reflect object  */
    ~Reflect();

    /*  getOperateType: get the type of the operation
     *  @return  the type fo the operation, i.e., mirror, rectangular or
     *  circlular array patterns  */
    int getOperateType() { return type; }

    /*  getMirrorPlane: the the information of the Mirror operation
     *  @return   the flags for the mirror planes  */
    bool *getMirrorPlane() { return mirrorFlag; }

    /*  getCoordsOfOrigin: get the coordinates of the new original point
     *  @return  the coordinates of the new orginal point  */
    double *getCoordsOfOrigin() { return coords; }

    /*  getAxisRotation: get the rotation along each axis
     *  @return  the rotation angles  */
    double *getRotation() { return angle; }

    /*  isUseFullModel: get the status that wheter the full model or current
     *  model is used for mirroring operation
     *  @return  whether the full model is used for mirroring  */
    bool isUseFullModel() { return isFullModel; }

private slots:
    /*  reset: reset the information in the dialog  */
    void reset();

    /*  save: save the dialog information to the local variables  */
    void save();
};

#endif  // REFLECT_H
