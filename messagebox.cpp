/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : messgebox.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 12th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "messagebox.h"

#include "ui_messagebox.h"
/*  ############################################################################
 *  constructor: create the Message box  */
MessageBox::MessageBox(QWidget* parent)
    : QDialog(parent), ui(new Ui::MessageBox) {
    //  create the UI interface
    ui->setupUi(this);

    //  connect the YES and NO button
    connect(ui->btnNo, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->btnYes, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->btnOk, &QPushButton::clicked, this, &QDialog::close);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destrucotr: destroy the Message box  */
MessageBox::~MessageBox() { delete ui; }

/*  ############################################################################
 *  showMessage: show the warnging, critical and question informaiton
 *  @argc  type: the type of the messagebox, 0 for warning, 1 for question,
 *               2 for critical
 *  @argc  winIcon: the icon of the window
 *  @argc  title: the title of the messagebox
 *  @argc  info: the information to show in the text browser  */
void MessageBox::showMessage(const int& type, const QString& winIcon,
                             const QString& title, const QString& info) {
    //  define the icon of the message box
    switch (type) {
        /*  warnging  messagebox  */
        case 0:
            pixmap.load(":/icons/warn.png");
            ui->btnYes->hide();
            ui->btnNo->hide();
            ui->btnOk->show();
            break;
        /*  question  messagebox  */
        case 1:
            pixmap.load(":/icons/question.png");
            ui->btnYes->show();
            ui->btnNo->show();
            ui->btnOk->hide();
            break;
        /*  critical  messagebox  */
        case 2:
            pixmap.load(":/icons/error.png");
            ui->btnYes->hide();
            ui->btnNo->hide();
            ui->btnOk->show();
            break;
    }
    //  set pixmap
    ui->icon->setPixmap(pixmap);

    //  set the windows Icon
    setWindowIcon(QIcon(winIcon));

    //  set the title of the messagebox
    setWindowTitle(title);

    //  set the information
    ui->info->setText(info);

    //  show the message box
    exec();
}
