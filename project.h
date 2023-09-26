/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : project.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 12th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef PROJECT_H
#define PROJECT_H

#include <QDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "messagebox.h"
#include "open.h"

namespace Ui {
class Project;
}

class Project : public QDialog {
    Q_OBJECT

private:
    Ui::Project* ui;     // UI interface

    Open* openDir;       // open work directory dialog
    Open* openPy;        // open python script
    Open* openProj;      // open project
    int openFlag;        // the flag to determine which button is clicked

    QString text;        // content of the line edit
    QString* workDir;    // work direction
    QString* name;       // name of the project
    QString pyScr;       // the path of python script
    QString savePath;    // the path of the *.pac file

    int dbType;          // the type of the database
    QSqlDatabase db;     // database to write the project information
    bool choice;         // the choice in message box
    bool ifDbExist;      // if the database is exist or not
    QString dbname;      // the name of database
    MessageBox* msgbox;  // message box to echo information

    //  some other needed variables
    QStandardItemModel* itemModel;  // the item model of the combobox in pacnano
    QStandardItem* item;            // item in ItemModel
public:
    /*  getDatabase: get the database object  */
    void getDatabase(QString& dbname);

    /*  getProjectName: get the name of the project  */
    QString*& getProjectName() { return name; };

    /*  getWorkDirectory: get the work directory of the project  */
    QString*& getWorkDirectory() { return workDir; }

    /*  getItemModel: get the item model object in Project  */
    QStandardItemModel* getItemModel() { return itemModel; }

private slots:
    /*  set information from OPEN dialog to the current */
    void setWorkDir();     // work directory
    void setPyScript();    // path for the python script
    void setOldProject();  // path for the old project

    /*  write project information to variable  */
    void writeProjectOld();  // old project
    void writeProjectNew();  // new project

public slots:
    /*  open the dialog to set the work directory  */
    void createProjectNew();  // new project
    void createProjectOld();  // old project

    /*  save project to local database  */
    void saveProject();
    /*  load project from local database  */
    void loadProject();
    /*  deletePorject: delete the project database  */
    void deleteProject();

public:
    /*  constructor: create a new project  */
    explicit Project(QWidget* parent = nullptr);

    /*  destructor: destroy the project  */
    ~Project();
};

#endif  // PROJECT_H
