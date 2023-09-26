/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Model.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 19th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef MODEL_H
#define MODEL_H

#include <QDialog>

#include "manager.h"
#include "messagebox.h"
#include "open.h"
#include "rename.h"
#include "set.h"

/*  class Model:
 *  the class to create the finite element model object, which includes
 *  the element, nodes, nodal sets, element sets, material and also the Neumann
 *  boundary conditons, and Dirichlet boundary conditions  */
namespace Ui {
class Model;
}

class Model : public QDialog {
    Q_OBJECT

private:
    Ui::Model* ui;       // UI interface

    Open* openModel;     // open the exist model object
    Open* openIo;        // open the input source file
    MessageBox* msgbox;  // message box to show the information
    Manager* mng;        // the manager object
    Rename* rename;      // the rename dialog
    bool isActiveMng;    // if the manager window is active

    QString* project;    // the project that will be attached to
    QString* workDir;    // the work directory

    int index;           // index of the current material
    bool editmode;       // if the model is in edit mode or creation
    QString text;        // temporary string for data operation

private:
    class ModelProperty;    // nested class of model property
    ModelProperty* proNew;  // the model property object for creation
    ModelProperty* proOld;  // the model property object for edition
    ModelProperty* proTem;  // the temporary object for model property
    ModelProperty* proCur;  // the current operated model property

    Set* set;               // the set object

private:
    QStandardItem* item;            // the item in the list view
    QStandardItemModel* itemModel;  // item model for list view

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
    /*  constructor: create the Model object  */
    explicit Model(QWidget* parent, QString*& projName, QString*& workPath);

    /*  destructor: destroy the Model object  */
    ~Model();

    /*  save the model information to the local database  */
    void saveModelDatabase();

    /*  get the model list  */
    QStandardItemModel* getModelList();

    /*  set currently operated model  */
    void setOperatedModel(int idx);

public slots:
    /*  createModel: create a new model  */
    void createModel();

    /*  showManager: show the model manager  */
    void showManager() {
        mng->show();
        isActiveMng = true;
    };

    /*  show Set create dialog */
    void createSetNode();

    /*  show Set create dialog  */
    void createSetElem();

    /*  show Set manager  */
    void showSetManager();

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

/*  class ModelProperty:
 *  the class to define the model property, i.e., the components, such as Set,
 *  Material and so on, over the model  */
class Model::ModelProperty {
public:
    bool ifUseNew;     // flag to determine the source to create a Model
    QString* name;     // the name of the model
    QString source;    // the source file
    int modelType;     // the type of the model
    int sourceType;    // source type of the model
    int createType;    // creation type of the model
    QString* project;  // the name of project
    QString* workDir;  // the current work directory

    Set* set;          // the set object

    QSqlDatabase db;   // the database object
    QString dbname;    // the name of database
    bool ifCreateDb;   // whether the database has been created or not

    bool assigned;     // whether the model is assigned
    bool ifExist;      // whether the database is exist or not?
    bool ifOverride;   // whether override the database

public:
    /*  constructor of the object  */
    ModelProperty(int _index, QString*& projName, QString*& workPah);

    /*  destructor of the object  */
    ~ModelProperty() {
        project = nullptr;
        workDir = nullptr;
        delete name;
        name = nullptr;
    };

    /*  save model information to the local database */
    void saveDatabase(QString& _workDir);

    /*  update the item order  */
    void updateItemOrder();

    /*  set Item name shown in model list view  */
    void updateItemName();
};

#endif  // MODEL_H
