/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : pacnano.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 14th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef PACNANO_H
#define PACNANO_H

#include <QMainWindow>
#include <QPushButton>

#include "matassign.h"
#include "material.h"
#include "model.h"
#include "open.h"
#include "project.h"
#include "remote.h"
#include "viewer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class pacnano;
}
QT_END_NAMESPACE

class pacnano : public QMainWindow {
    Q_OBJECT

private:
    Ui::pacnano *ui;       // ui interface
    Remote *remote;        // remote server login UI
    Open *openDir;         // open the directory
    Project *project;      // pacnano project
    Model *model;          //  model object
    Material *material;    // material dialog
    Viewer *win;           // render window
    MatAssign *matAssign;  // material assignment

public:
    /*  constructor :
        create the pacnano mainwindow  */
    pacnano(QWidget *parent = nullptr);
    /*  destructor:
        destroy the pacnano mainwindow  */
    ~pacnano();

private:
    /*  setupProject: setup the project related function  */
    void setupProject();

    /*  setupViewportSwitch: setup the viewport combobox  */
    void setupViewportSwitch();

    /*  setupModel: setup the model related environment  */
    void setupModel();

    /*  setupMaterialDialog: setup the material create dialog  */
    void setupMaterialCreation();
};
#endif
