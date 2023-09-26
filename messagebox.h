/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : messgebox.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 12th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>

namespace Ui {
class MessageBox;
}

class MessageBox : public QDialog {
    Q_OBJECT

private:
    Ui::MessageBox* ui;  // ui interface
    QPixmap pixmap;

public:
    /*  constructor */
    explicit MessageBox(QWidget* parent = nullptr);

    /*  destructor: destroy the MessageBox*/
    ~MessageBox();

    /*  showMessage: show the warnging, critical and question informaiton
     *  @argc  type: the type of the messagebox, 0 for warning, 1 for question,
     *               2 for critical
     *  @argc  winIcon: the icon of the window
     *  @argc  title: the title of the messagebox
     *  @argc  info: the information to show in the text browser  */
    void showMessage(const int& type, const QString& winIcon,
                     const QString& title, const QString& info);
};

#endif  // MESSAGEBOX_H
