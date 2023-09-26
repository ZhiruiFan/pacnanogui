/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Manager.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 19th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef MANAGER_H
#define MANAGER_H

#include <QDialog>
#include <QItemSelectionModel>
#include <QStandardItemModel>

namespace Ui {
class Manager;
}

class Manager : public QDialog {
    Q_OBJECT

private:
    Ui::Manager *ui;                   // UI interface
    QItemSelectionModel *selectModel;  // the selection model of the listview
    QStandardItemModel *itemModel;     // the model to show th list view
    QModelIndex index;                 // the selected item in list view

signals:
    void closed();               //  the close signal of the window
    void shown();                // the signal when the dialog is shown
    void signalCreate();         // the signal for creating a item
    void signalEdit(int idx);    // the signal for editing an item
    void signalDelete(int idx);  // the signla for deleting an item
    void signalRename(int idx);  // the signal for renaming an item

public:
    /*  constructor: create a Manager object  */
    explicit Manager(QWidget *parent, const QString &winTitle,
                     const QString &icon, QStandardItemModel *&itemModel);

    /*  destructor: destroy the Manager object  */
    ~Manager();

private slots:
    /*  get the selected item in the list view  */
    void getSelectedIndex();

    /*  slots of the button with respect to model selection  */
    void onClickEdit();    // edit button
    void onClickDelete();  // delete button
    void onClickRename();  // rename button

protected:
    /*  closeEvent: override the close event  */
    void closeEvent(QCloseEvent *event) override {
        emit closed();
        QDialog::closeEvent(event);
    }
    /*  showEvent: override the show event  */
    void showEvent(QShowEvent *event) override {
        emit shown();
        QDialog::showEvent(event);
    }
};

#endif  // MANAGER_H
