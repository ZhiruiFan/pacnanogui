/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : project.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 12th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "project.h"

#include "ui_project.h"

/*  ############################################################################
 *  constructor: create the Project object  */
Project::Project(QWidget *parent) : QDialog(parent), ui(new Ui::Project) {
    /*  Create UI  */
    ui->setupUi(this);

    //  create the message box
    msgbox = new MessageBox(this);
    connect(msgbox, &QDialog::accepted, this, [&]() { choice = true; });
    connect(msgbox, &QDialog::rejected, this, [&]() { choice = false; });

    /*  initialize the database  */
    db      = QSqlDatabase::addDatabase("QSQLITE", "PROJECT");
    name    = new QString("Project-0");
    workDir = new QString(QDir::currentPath());
    pyScr   = "*.py";

    /*  Open dialog  */
    openDir = new Open(this, 0);
    //  open work directory
    connect(ui->btnOpenDirNew, &QToolButton::clicked, this, [&]() {
        //  set dialog flag
        openFlag = 0;
        //  show the dialog
        openDir->exec();
    });
    connect(ui->btnOpenDirOld, &QToolButton::clicked, this, [&]() {
        //  set dialog flag
        openFlag = 1;
        //  show the dialog
        openDir->exec();
    });
    //  open old project
    openProj = new Open(this, 2);
    connect(ui->btnOpenProjOld, &QToolButton::clicked, openProj,
            &QDialog::exec);
    //  open python script
    openPy = new Open(this, 3);
    connect(ui->btnOpenScrNew, &QToolButton::clicked, openPy, &QDialog::exec);
    //  OK button for OPEN dialog
    connect(openDir, &QDialog::accepted, this, &Project::setWorkDir);
    connect(openProj, &QDialog::accepted, this, &Project::setOldProject);
    connect(openPy, &QDialog::accepted, this, &Project::setPyScript);

    /*  Cancel button  */
    connect(ui->btnCancelNew, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->btnCancelOld, &QPushButton::clicked, this, &QDialog::close);

    /*  OK buton  */
    connect(ui->btnOkNew, &QPushButton::clicked, this,
            &Project::writeProjectNew);
    connect(ui->btnOkOld, &QPushButton::clicked, this,
            &Project::writeProjectOld);

    //  Item model to show the project name in the pacnano gui
    itemModel = new QStandardItemModel(this);
    item      = new QStandardItem(*name);
    itemModel->appendRow(item);
    item = nullptr;
}
/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor: destroy the Project object  */
Project::~Project() {
    delete ui;
    delete openDir;
    delete openPy;
    delete openProj;
    delete msgbox;
}

/*  ############################################################################
 *  set information from OPEN dialog to the current */
void Project::setWorkDir() {
    //  get the information from the OPEN dialog
    openDir->getSelectContent(text);
    //  set the information
    switch (openFlag) {
        /*  Work directory for new project  */
        case 0:
            ui->workDirNew->setText(text);
            break;
        /*  Work directory for old project  */
        case 1:
            ui->workDirOld->setText(text);
            break;
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  set information from OPEN dialog to the current */
void Project::setPyScript() {
    //  get the information from the OPEN dialog
    openPy->getSelectContent(text);
    ui->pyScrNew->setText(text);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  set information from OPEN dialog to the current */
void Project::setOldProject() {
    //  get the path of the *pac
    openProj->getSelectContent(text);
    //  save the *.pac file path
    savePath = text;
    //  set the project name and workdirectory
    QStringList splitText = text.split("/");
    ui->projNameOld->setText(text.split("/").last());
    ui->workDirOld->setText(splitText.mid(0, splitText.size() - 1).join("/"));
}

/*  ############################################################################
 *  createPorjectNew: open the curerent ui to create a new project  */
void Project::createProjectNew() {
    // set the page
    ui->createProject->setCurrentIndex(0);
    //  show the dialog
    show();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  createProjectOld: open current UI to open a old project  */
void Project::createProjectOld() {
    // set the page
    ui->createProject->setCurrentIndex(1);
    //  show the dialog
    show();
}

/*  ############################################################################
 *  writeProjectNew: get  dialog information and write them to variables*/
void Project::writeProjectNew() {
    //  check the data completation of the project
    switch (openFlag) {
        case 0:
            //  check the project name
            if (ui->projNameNew->text().isEmpty()) {
                msgbox->showMessage(0, ":/icons/project.png",
                                    "Project Creation",
                                    "Please input the name of the project.");
                return;
            }
            //  check the workdir
            if (ui->workDirNew->text().isEmpty()) {
                msgbox->showMessage(
                    0, ":/icons/project.png", "Project Creation",
                    "Please select or input the path of work directory.");
                return;
            }
            if (ui->pyScrNew->text().isEmpty()) {
                msgbox->showMessage(
                    0, ":/icons/project.png", "Project Creation",
                    "Please select or input the python script.");
                return;
            }
            break;
        case 1:
            if (ui->projNameOld->text().isEmpty()) {
                msgbox->showMessage(0, ":/icons/project.png",
                                    "Project Creation",
                                    "Please input the name of the project.");
                return;
            }
            if (ui->workDirOld->text().isEmpty()) {
                msgbox->showMessage(
                    0, ":/icons/project.png", "Project Creation",
                    "Please select or input the path of work directory.");
                return;
            }
    }

    //  check the path of python script

    //  set the basic project information
    *workDir = ui->workDirNew->text();
    *name    = ui->projNameNew->text();
    pyScr    = ui->pyScrNew->text();
    //  assign database type
    if (ui->useAsciiNew->isChecked()) dbType = 0;
    if (ui->useBinNew->isChecked()) dbType = 1;
    if (ui->useHdf5New->isChecked()) dbType = 2;
    //  update the item model
    item = itemModel->item(0, 0);
    item->setText(*name);
    item = nullptr;
    //  close the dialog
    accept();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  writeProjectOld: get  dialog information and write them to variables*/
void Project::writeProjectOld() {
    //  assign the basic project information
    *workDir = ui->workDirOld->text();
    *name    = ui->projNameOld->text();
    //  set the database type
    if (ui->useAsciiOld->isChecked()) dbType = 0;
    if (ui->useBinOld->isChecked()) dbType = 1;
    if (ui->useHdf5Old->isChecked()) dbType = 2;
    //  read the project information
    loadProject();
    //  close the dialog
    accept();
}

/*  #########################################################################
 *  saveProject: save project to local database  */
void Project::saveProject() {
    //  set the database name
    dbname = *workDir + "/" + *name + ".pac";
    //  create the database
    db.setDatabaseName(dbname);
    if (!db.open()) {
        qDebug() << "Failed to open the database" << db.lastError().text();
    }

    //  create the query object
    QSqlQuery query(db);

    //  check if the table "PROJECT" exists
    query.prepare(
        "SELECT COUNT(*) FROM sqlite_master WHERE type=\"table\""
        "AND name=\"project\";");
    if (query.exec() && query.next()) {
        //  check the table is exist or not?
        ifDbExist = query.value(0).toInt() > 0 ? true : false;
    }

    //  handle the exist
    if (ifDbExist) {
        // prepare the message box
        msgbox->showMessage(1, ":/icons/project_sve.png", "Saving Project",
                            "The project\n" + dbname +
                                "\nalreaty exist. Do you want to override it?");
        //  check if the "Override" is accepted
    }

    //  if doesnot exist the table PROJECT, then write new
    if (!ifDbExist) {
        query.exec(
            "CREATE TABLE IF NOT EXISTS project "
            "(id INTEGER PRIMARY KEY, "
            "key TEXT, "
            "value TEXT)");
        if (query.lastError().isValid()) {
            qDebug() << "Table creation failed:" << query.lastError().text();
        }

        //  define the insert command
        query.prepare("INSERT INTO project (id, key, value) VALUES (?, ?, ?)");

        //  insert project name
        query.bindValue(0, 0);
        query.bindValue(1, "PROJECT_NAME");
        query.bindValue(2, *name);
        query.exec();
        //  insert the work directory
        query.bindValue(0, 1);
        query.bindValue(1, "WORK_DIRECTORY");
        query.bindValue(2, *workDir);
        query.exec();
        //  insert the work directory
        query.bindValue(0, 2);
        query.bindValue(1, "PYTHON_SCRIPT");
        query.bindValue(2, pyScr);
        query.exec();
        if (query.lastError().isValid()) {
            qDebug() << "Data insertion failed:" << query.lastError().text();
        }
        //  close the database
        query.finish();
        query.clear();
        qDebug() << "a new database has been created.";
    }
    //  the TABLE PROJECT exist, and will override the original
    else {
        if (choice) {
            //  SQLite update command
            query.prepare(
                "UPDATE PROJECT SET key = :key, "
                "value = :value WHERE id = :id");
            //  update the project name
            query.bindValue(":key", "PROJECT_NAME");
            query.bindValue(":value", *name);
            query.bindValue(":id", 0);
            query.exec();
            //  update the work directory
            query.bindValue(":key", "WORK_DIRECTORY");
            query.bindValue(":value", *workDir);
            query.bindValue(":id", 1);
            query.exec();
            //  update the python script
            query.bindValue(":key", "PYTHON_SCRIPT");
            query.bindValue(":value", pyScr);
            query.bindValue(":id", 2);
            query.exec();
            //  check the errors
            if (query.lastError().isValid()) {
                qDebug() << "Data insertion failed:"
                         << query.lastError().text();
            }
            //  close the database
            query.finish();
            query.clear();
            qDebug() << "the database has been overrided.";
        }
    }
    //  close the database
    QSqlDatabase::database().close();
    db.close();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  loadProject: load project from local database  */
void Project::loadProject() {
    //  set the database name
    dbname = savePath;
    //  create the database
    db.setDatabaseName(dbname);
    if (!db.open()) {
        qDebug() << "Failed to open the database" << db.lastError().text();
    }
    //  load the information of project
    QSqlQuery query(db);
    query.prepare("SELECT * FROM project;");
    //  perform the query command
    query.exec();
    //  get the name of the project
    query.next();
    *name = query.value(2).toString();
    qDebug() << *name;
    //  get the work directory
    query.next();
    *workDir = query.value(2).toString();
    qDebug() << workDir;
    //  get the python script
    query.next();
    pyScr = query.value(2).toString();
    qDebug() << pyScr;

    //  close the query
    query.finish();
    query.clear();

    //  close the database
    db.close();
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  deletePorject: delete the project database  */
void Project::deleteProject() {
    //  prepare the message box
    msgbox->showMessage(
        1, ":/icons/project_sve.png", "Remove Project",
        "The project \"" + *name + "\"will be removed.\n Continue or not?");
    //  handle the "delete" signla
    if (choice) {
        QFile file(dbname);
        if (file.remove()) {
            qDebug() << "The file has been removed from the local.";
        }
    }
}
