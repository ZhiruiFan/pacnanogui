/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Set.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 19th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef SET_H
#define SET_H

#include <QDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "manager.h"
#include "messagebox.h"
#include "open.h"
#include "rename.h"

/*  ############################################################################
 *  class Set:
 *  the class is used to create a set object, which includes the node and
 *  element sets.
 *  Notably, this class is belong to the modlus Model, and this object will be
 *  invoked by Model directly.  */
namespace Ui {
class Set;
}

class Set : public QDialog {
    Q_OBJECT

public:
private:
    Ui::Set* ui;         // the UI interface

    Open* openIo;        // open the input source file
    MessageBox* msgbox;  // message box to show the information
    Manager* mng;        // the manager object
    Rename* rename;      // the rename dialog
    bool isActiveMng;    // if the manager window is active

    QString* model;      // the model that the set is belong to
    QString* project;    // the project that will be attached to
    QString* workDir;    // the work directory

    int index;           // index of the current material
    bool editmode;       // if the model is in edit mode or creation
    QString text;        // temporary string for data operation

    class Property;      // the set property
    Property* proNew;    // the model property object for creation
    Property* proOld;    // the model property object for edition
    Property* proTem;    // the temporary object for model property

private:
    QStandardItem* item;            // the item in the list view
    QStandardItemModel* itemModel;  // item for model list view

private:
    /*  setupSourceSelect: setup the environment for selecting a source type
     *  to create a Model */
    void setupSourceSelect();

    /*  setupMessageBox: setup the environment of the Message Box  */
    void setupMessageBox();

    /*  setupDialogButton: setup the button of in the dialog  */
    void setupDialogButton();

    /*  setupManager: setup the model manager object  */
    void setupManager();

    /*  setup Rename dialog  */
    void setupRenameDialog();

private slots:
    /*  saveModelDialog: save the dialog information to the variables  */
    void saveModelDialog();

    /*  loadModelDialog: load the model information to the dialog  */
    void loadModelDialog(int idx);

    /*  confirmDialog: check the dialog information is complete  */
    bool confimDialog();

    /*  load the model information from the local database  */
    void loadModelDatabase();

    /*  getModelName: get the name of the selected model  */
    void getModelName(int idx);

    /*  setModelName: set the new name to the selected model  */
    void setModelName();

    /*  deleteModel: delete the selected model  */
    void deleteModel(int idx);

public:
    /*  constructor: create the Set object  */
    explicit Set(QWidget* parent, QString*& projName, QString*& modelName,
                 QString*& workPath);

    /*  destructor: destroy the Set object  */
    ~Set();

    /*  save the model information to the local database  */
    void saveModelDatabase();

public slots:
    /*  createSet: create a new model  */
    void createSetNode();

    /*  createSetElem: create a element set  */
    void createSetElem();

    /*  showManager: show the model manager  */
    void showManager() {
        mng->show();
        isActiveMng = true;
    };

protected:
    /*  closeEvent: override the close event  */
    void closeEvent(QCloseEvent* event) override {
        if (isActiveMng) mng->show();
        QDialog::closeEvent(event);
    }
    /*  showEvent: override the show event  */
    void showEvent(QShowEvent* event) override {
        if (isActiveMng) mng->hide();
        QDialog::showEvent(event);
    }
};

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  class Set::Property
 *  this class is used to define the set property used in Set object.  */
class Set::Property {
public:
    QString name;       // the name of the set
    QString* model;     // the model that the model is belong to
    QString* project;   // the set that is belong to
    QString* workDir;   // the workdirectory
    int type;           // the type of the set, i.e., node or element
    QString source;     // the path of the source
    int createType;     // the type in creation the set
    QVector<int> data;  // the index of the data

    QSqlDatabase db;    // the database object
    QString dbname;     // the name of database
    bool ifCreateDb;    // whether the database has been created or not

    bool assigned;      // whether the model is assigned
    bool ifExist;       // whether the database is exist or not?
    bool ifOverride;    // if override the exist database

private:
    MessageBox* msgbox;  // the message box to show the information

public:
    /*  constructor: create the set property object  */
    explicit Property(int index, QString*& projName, QString*& modelName,
                      QString*& path);
    /*  destructor: destroy the set property object  */
    ~Property();

    /*  saveDatabase: save the set data to the local database  */
    void saveDatabase(QString& workDir);
};

#endif  // SET_H
