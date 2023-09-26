/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : rename.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 16th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef RENAME_H
#define RENAME_H

#include <QDialog>

namespace Ui {
class Rename;
}

class Rename : public QDialog {
    Q_OBJECT

private:
    Ui::Rename *ui;  // UI interface

public:
    /*  setNameOriginal: set the original name  */
    void setNameOriginal(QString &_nameOrig);

    /*  getNameNew: get the new object name  */
    void getNameNew(QString &_nameNew);

public:
    /*  constructor: create a new rename object  */
    explicit Rename(QWidget *parent = nullptr);

    /*  destructor: destroy the old name  */
    ~Rename();
};

#endif  // RENAME_H
