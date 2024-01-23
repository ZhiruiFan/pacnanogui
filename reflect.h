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

public:
    /*  constructor: create the Reflect object  */
    explicit Reflect(QWidget *parent = nullptr);
    /*  destructor: destroy the Reflect object  */
    ~Reflect();

    /*  getMirrorPlane: the the information of the Mirror operation
     *  @return   the flags for the mirror planes  */
    bool *getMirrorPlane() { return mirrorFlag; }

private slots:
    /*  reset: reset the information in the dialog  */
    void reset();

    /*  save: save the dialog information to the local variables  */
    void save();
};

#endif  // REFLECT_H
