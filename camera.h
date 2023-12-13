/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : camera.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : December 12th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef CAMERA_H
#define CAMERA_H

#include <QDialog>

namespace Ui {
class Camera;
}

/*  CLASS Camera: this class is used to define the configuration for the camera
 *  in the viewport, which includes:
 *  [1] define the movement distance increment
 *  [2] define the rotation angle increment
 *  [3] define the zoom scale increment  */
class Camera : public QDialog {
    Q_OBJECT

private:
    Ui::Camera *ui;    // dialog interface
    double moveInc;    // moving distance increment
    double rotateInc;  // rotating angle increment
    double zoomInc;    // zooming scacle increment

public:
    /*  constructor: create the Camera object  */
    explicit Camera(QWidget *parent = nullptr);

    /*  destructor: deallocate the Camera object  */
    ~Camera();

    /*  getMoveIncrement: get the moving distance increment
     *  @return  the distance increment  */
    double getMoveIncrement() { return moveInc; };

    /*  getRotateIncrement: get the rotation angle increment of the camera
     *  @return  the angle increment  */
    double getRotateIncrement() { return rotateInc; };

    /*  getZoomIncrement: get the zoom scale increment of the camera  */
    double getZoomIncrement() { return zoomInc; };
};

#endif  // CAMERA_H
