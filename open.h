/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : opendir.hpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 11th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef OPEN_H
#define OPEN_H

#include <QDialog>
#include <QFileSystemModel>
#include <QListView>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <vector>

namespace Ui {
class Open;
}

class Open : public QDialog {
    Q_OBJECT

private:
    QFileSystemModel* dir;        // model to read the directory
    QFileSystemModel* file;       // modle to read the files in current

    int flagView;                 // flag to determine which viewer is actived
    int flagDiag;                 // flag to detemine the dialog types
    QString pathCur;              // current directory path
    QModelIndex rootIndex;        // file root index
    QDir parentDir;               // file parent index

    std::vector<int> filterFlag;  // flags for filter

private slots:
    /*  show folders in different styles or formats */
    void showListView();   // using list view
    void showTreeView();   // using tree view
    void showTableView();  // using table view

    /*  double clicks to open the folder  */
    void openListView(const QModelIndex& index);   // open folder in list view
    void openTreeView(const QModelIndex& index);   // open folder in tree view
    void openTableView(const QModelIndex& index);  // open folder in table view

    /*  change the driver and link the viewer */
    void changeDriver(const int index);

    /*  Back to the parent folder  */
    void navigateUp();

    /*  click to select a folder  */
    void selectFolderInListView(const QModelIndex& index);   // list view
    void selectFolderInTreeView(const QModelIndex& index);   // tree view
    void selectFolderInTableView(const QModelIndex& index);  // table view

    /*  reset the directory to the home  */
    void resetHome();

    /*  switch selction types  */
    void switchSelctionType(int index);

public:
    /*  Constructor: create a dialog to open a directory  */
    explicit Open(QWidget* parent = nullptr, const int type = 0);

    /*  Destructor: destroy the dialog that is used to open a directory  */
    ~Open();

    /*  Get the content of the selected file or path  */
    void getSelectContent(QString& text);

protected:
    void showEvent(QShowEvent* event) override {
        QDialog::showEvent(event);
        showListView();
        resetHome();
    }

private:
    Ui::Open* ui;
};

#endif  // OPEN_H
