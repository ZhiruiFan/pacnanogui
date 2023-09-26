/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : opendir.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 11th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "open.h"

#include "ui_open.h"

/*  ############################################################################
 *  Constructor: create the OpenDir object  */
Open::Open(QWidget *parent, const int type)
    : QDialog(parent), ui(new Ui::Open) {
    /*  CREATE UI  */
    ui->setupUi(this);

    /*  DRIVERS  */
    pathCur = QDir::currentPath();
    dir     = new QFileSystemModel;
    dir->setRootPath(pathCur);
    dir->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    /*  FILE EXPLORER  */
    file = new QFileSystemModel;
    file->setRootPath(QDir::currentPath());
    //  assign the diag types
    flagDiag = type;
    //  define filters
    QStringList filter;
    filter << "All folders (*)"
           << "All files (*)"
           << "Project (*.pac)"
           << "Model (*.model)"
           << "Python Script (*.py, *.pyc)"
           << "Pacnano input (*.in)"
           << "Pacnano output (*.ou)"
           << "Pacnano in&out (*.in, *.ou)";
    //  initialize the filter flags
    filterFlag.clear();
    ui->fileFilter->clear();
    //  define the combobox items
    switch (flagDiag) {
        /*  folders  */
        case 0:
            filterFlag.push_back(0);
            break;
        /*  all files  */
        case 1:
            filterFlag.push_back(1);
            filterFlag.push_back(2);
            filterFlag.push_back(3);
            filterFlag.push_back(4);
            filterFlag.push_back(5);
            filterFlag.push_back(6);
            break;
        /*  project */
        case 2:
            filterFlag.push_back(2);
            filterFlag.push_back(1);
            break;
        /*  Model */
        case 3:
            filterFlag.push_back(3);
            filterFlag.push_back(1);
            break;
        /*  python scripts  */
        case 4:
            filterFlag.push_back(4);
            filterFlag.push_back(1);
            break;
        /*  pacnano input  */
        case 5:
            filterFlag.push_back(5);
            filterFlag.push_back(1);
            break;
        /*  pacnano output  */
        case 6:
            filterFlag.push_back(6);
            filterFlag.push_back(1);
            break;
        /*  all input and out files  */
        case 7:
            filterFlag.push_back(7);
            filterFlag.push_back(5);
            filterFlag.push_back(6);
            filterFlag.push_back(3);
            filterFlag.push_back(2);
            filterFlag.push_back(1);
            break;
        default:  // error occurs
            QMessageBox::critical(this, "ERROR",
                                  "The type of the diaglog type is wrong.");
    }
    //  set the items to the combobox
    for (std::vector<int>::iterator it = filterFlag.begin();
         it != filterFlag.end(); ++it) {
        ui->fileFilter->addItem(filter[*it]);
    }
    //  initialize the filter
    switchSelctionType(0);

    /*  VIEWERS  */
    showListView();

    /*  COMBOBOX  */
    ui->dirExplorer->setModel(dir);
    ui->dirExplorer->setModelColumn(0);
    connect(ui->dirExplorer, &QComboBox::currentIndexChanged, this,
            &Open::changeDriver);

    /*  CANCEL BUTTION  */
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::close);
    /*  OK button  */
    connect(ui->btnOk, &QPushButton::clicked, this, &QDialog::accept);

    /*  VIEWER CHANGING  */
    connect(ui->btnList, &QPushButton::clicked, this, &Open::showListView);
    connect(ui->btnTree, &QPushButton::clicked, this, &Open::showTreeView);
    connect(ui->btnTable, &QPushButton::clicked, this, &Open::showTableView);

    /*  DOUBLE CLICKS  */
    connect(ui->listView, &QListView::doubleClicked, this, &Open::openListView);
    connect(ui->treeView, &QTreeView::doubleClicked, this, &Open::openTreeView);
    connect(ui->tableView, &QTableView::doubleClicked, this,
            &Open::openTableView);

    /*  NAVIGATE UP BUTTON  */
    connect(ui->btnBack, &QPushButton::clicked, this, &Open::navigateUp);

    /*  SELECT A FOLDER  */
    connect(ui->listView, &QListView::clicked, this,
            &Open::selectFolderInListView);
    connect(ui->treeView, &QTreeView::clicked, this,
            &Open::selectFolderInListView);
    connect(ui->tableView, &QTableView::clicked, this,
            &Open::selectFolderInListView);

    /*  RESET PATH TO HOME  */
    connect(ui->btnHome, &QPushButton::clicked, this, &Open::resetHome);

    /*  FOLDER FILTER  */
    connect(ui->fileFilter, &QComboBox::currentIndexChanged, this,
            &Open::switchSelctionType);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Destructor: destroy the OpenDir object  */
Open::~Open() {
    delete dir;   // directory system
    delete file;  // file system
    delete ui;    // UI interface
}

/*  ##############################  LIST VIEWER  ###############################
 *  showDirListView: show the directory information using a list */
void Open::showListView() {
    //  assgn the active viewer flag
    flagView = 0;

    //  set the file system model
    file->setRootPath(pathCur);
    ui->listView->setModel(file);
    rootIndex = file->index(pathCur);
    ui->listView->setRootIndex(rootIndex);

    //  show the viewer
    ui->listView->show();
    ui->tableView->hide();
    ui->treeView->hide();

    //  update the folder name
    ui->fileName->setText(pathCur);

    //  judge the navigateUp
    if (!(rootIndex.parent().isValid())) {
        ui->btnBack->setDisabled(true);
    } else {
        ui->btnBack->setEnabled(true);
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  SLOT: double click to open a folder in list  */
void Open::openListView(const QModelIndex &index) {
    //  open the folder
    pathCur   = file->fileInfo(index).absoluteFilePath();
    rootIndex = file->setRootPath(pathCur);
    ui->listView->setRootIndex(rootIndex);
    ui->btnBack->setDisabled(false);

    //  update the folder name
    ui->fileName->setText(pathCur);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  click to select a folder in list view  */
void Open::selectFolderInListView(const QModelIndex &index) {
    //  get the path
    pathCur = file->fileInfo(index).absoluteFilePath();
    //  update the folder name
    ui->fileName->setText(pathCur);
}

/*  ##############################  TREE VIEWER  ###############################
 *  showDirTreeView: show the directory informaiton using a tree */
void Open::showTreeView() {
    //  assgn the active viewer flag
    flagView = 1;

    //  show the directory tree
    file->setRootPath(pathCur);
    ui->treeView->setModel(file);
    ui->treeView->setHeaderHidden(true);
    for (int i = 1; i < file->columnCount(); ++i) {
        ui->treeView->setColumnHidden(i, true);
    }
    rootIndex = file->index(pathCur);
    ui->treeView->setRootIndex(rootIndex);

    //  show the list view and hide the other two
    ui->treeView->show();
    ui->tableView->hide();
    ui->listView->hide();

    //  update the folder name
    ui->fileName->setText(pathCur);

    //  judge the navigateUp
    if (!(rootIndex.parent().isValid())) {
        ui->btnBack->setDisabled(true);
    } else {
        ui->btnBack->setEnabled(true);
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  SLOT: click and open a folder in tree  */
void Open::openTreeView(const QModelIndex &index) {
    //  open folder
    pathCur   = file->fileInfo(index).absoluteFilePath();
    rootIndex = file->setRootPath(pathCur);
    ui->treeView->setRootIndex(rootIndex);
    ui->btnBack->setDisabled(false);

    //  update the folder name
    ui->fileName->setText(pathCur);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  click to select a folder in tree view  */
void Open::selectFolderInTreeView(const QModelIndex &index) {
    //  get the path
    pathCur = file->fileInfo(index).absoluteFilePath();
    //  update the folder name
    ui->fileName->setText(pathCur);
}

/*  ###############################  TABLE VIEWER  #############################
 *  showDirTableView: show the directory information using a table  */
void Open::showTableView() {
    //  set the viewer flag
    flagView = 2;

    //  assign the FileSystemModel information
    file->setRootPath(pathCur);
    ui->tableView->setModel(file);
    rootIndex = file->index(pathCur);
    ui->tableView->setRootIndex(rootIndex);
    ui->tableView->setShowGrid(false);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->hideColumn(1);
    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);

    //  show the list view and hide the other two
    ui->tableView->show();
    ui->treeView->hide();
    ui->listView->hide();

    //  update the folder name
    ui->fileName->setText(pathCur);

    //  judge the navigateUp
    if (!(rootIndex.parent().isValid())) {
        ui->btnBack->setDisabled(true);
    } else {
        ui->btnBack->setEnabled(true);
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  SLOT: click and open a folder in tree  */
void Open::openTableView(const QModelIndex &index) {
    //  open folder
    pathCur   = file->fileInfo(index).absoluteFilePath();
    rootIndex = file->setRootPath(pathCur);
    ui->tableView->setRootIndex(rootIndex);
    ui->btnBack->setEnabled(true);

    //  update the folder name
    ui->fileName->setText(pathCur);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  click to select a folder in table view  */
void Open::selectFolderInTableView(const QModelIndex &index) {
    //  get the path
    pathCur = file->fileInfo(index).absoluteFilePath();
    //  update the folder name
    ui->fileName->setText(pathCur);
}

/*  ############################################################################
    changeDriver:
        show the dirver concents when the information of QComboBox "dirExplorer"
        is changed. */
void Open::changeDriver(int index) {
    //  get the path of the QComboBox
    pathCur = file->fileInfo(ui->dirExplorer->model()->index(index, 0))
                  .absoluteFilePath();
    rootIndex = file->index(pathCur);
    //  show the path viewer
    switch (flagView) {
        case 0:
            ui->listView->setRootIndex(rootIndex);
            break;
        case 1:
            ui->treeView->setRootIndex(rootIndex);
            break;
        case 2:
            ui->tableView->setRootIndex(rootIndex);
            break;
        defalut:
            exit(-1);
    }

    //  judge the navigateUp
    if (!(rootIndex.parent().isValid())) {
        ui->btnBack->setDisabled(true);
    } else {
        ui->btnBack->setEnabled(true);
    }
}

/*  ############################################################################
 *  navigateUp: back to the parent folder  */
void Open::navigateUp() {
    //  check the parent path is valid or not
    if (rootIndex.parent().isValid()) {
        pathCur = file->filePath(rootIndex.parent());
        ui->btnBack->setEnabled(true);
        switch (flagView) {
            case 0:
                showListView();
                break;
            case 1:
                showTreeView();
                break;
            case 2:
                showTableView();
                break;
            default:
                exit(-1);
        }
    } else {
        ui->btnBack->setDisabled(true);
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  reset the directory to the home  */
void Open::resetHome() {
    //  get the home path
    pathCur = QDir::currentPath();
    ui->btnBack->setEnabled(true);

    //  update the viewer
    switch (flagView) {
        case 0:
            showListView();
            break;
        case 1:
            showTreeView();
            break;
        case 2:
            showTableView();
            break;
        default:
            exit(-1);
    }

    //  judge the navigateUp
    if (!(rootIndex.parent().isValid())) {
        ui->btnBack->setDisabled(true);
    } else {
        ui->btnBack->setEnabled(true);
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  switch selction types  */
void Open::switchSelctionType(int index) {
    //  temporary variables
    QStringList filter;
    //  define filters
    switch (filterFlag[index]) {
        /*  folders  */
        case 0:
            file->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
            break;
        /*  all files  */
        case 1:
            filter << "*";
            file->setNameFilters(filter);
            file->setNameFilterDisables(false);
            break;
        /*  Project  */
        case 2:
            filter << "*.pac";
            file->setNameFilters(filter);
            file->setNameFilterDisables(false);
            break;
        /*  Model  */
        case 3:
            filter << "*.model";
            file->setNameFilters(filter);
            file->setNameFilterDisables(false);
            break;
        /*  Python script  */
        case 4:
            filter << "*.py"
                   << "*.pyc";
            file->setNameFilters(filter);
            file->setNameFilterDisables(false);
            break;
        /*  Pacnano input files  */
        case 5:
            filter << "*.in";
            file->setNameFilters(filter);
            file->setNameFilterDisables(false);
            break;
        /*  Pacnano output files  */
        case 6:
            filter << "*.ou";
            file->setNameFilters(filter);
            file->setNameFilterDisables(false);
            break;
        /*  Pacnano input and out files  */
        case 7:
            filter << "*.in"
                   << "*.ou";
            file->setNameFilters(filter);
            file->setNameFilterDisables(false);
            break;
        default:  // error occurs
            QMessageBox::critical(this, "ERROR",
                                  "The type of the diaglog type is wrong.");
    }
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  getSelectContent: get the content of the selected file or path  */
void Open::getSelectContent(QString &text) { text = ui->fileName->text(); }

/*============================================================================*/
