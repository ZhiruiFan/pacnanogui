/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : pacnano.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : September 14th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "pacnano.h"

#include "./ui_pacnano.h"

pacnano::pacnano(QWidget* parent) : QMainWindow(parent), ui(new Ui::pacnano) {
    /*  CREATE UI  */
    ui->setupUi(this);

    /*  Open forder  */
    openDir = new Open(this, 0);
    connect(ui->actProjOld, &QAction::triggered, openDir, &QDialog::show);
    connect(ui->actExit, &QAction::triggered, this, &QMainWindow::close);

    /*  Model tree  */
    ui->modelTree->expandAll();

    /*  Create the small widget  */
    setupProject();           // create project
    setupModel();             // setup model environment
    setupMaterialCreation();  // material creation
    setupViewportSwitch();    // viewport setting
    setupRenderWindow();      // set up the vtk render window

    /*  show the welcome window  */
    ui->mainView->setCurrentIndex(0);
    ui->viewWindow->hide();

    /*  hide the message tool bar  */
    ui->innerTool->setCurrentIndex(0);
}

/*  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  destructor:
        destroy the pacnano mainwindow  */
pacnano::~pacnano() {
    delete ui;        // UI interface
    delete remote;    // remote server
    delete openDir;   // pen directory dialog
    delete project;   // project object
    delete material;  // material object
    delete renWin;    // render window
}

/*  ############################################################################
 *  setupViewportSwitch: setup the viewport combobox  */
void pacnano::setupViewportSwitch() {
    //  define the viewport list
    QStringList vpList;
    vpList << "Project"
           << "Model"
           << "Analysis"
           << "Optimization"
           << "Postprocess"
           << "Monitor";
    //  add the viewport list
    ui->vpSwtich->addItems(vpList);
    ui->vpSwtich->setCurrentIndex(0);
    ui->vpToolBar->setCurrentIndex(0);

    // Function: change the viewport using ComboBox
    connect(ui->vpSwtich, &QComboBox::currentIndexChanged, this,
            [&](int index) { ui->vpToolBar->setCurrentIndex(index); });
    //  Function: menu action to show project viewport
    connect(ui->actVpProject, &QAction::triggered, this, [&]() {
        ui->vpToolBar->setCurrentIndex(0);
        ui->vpSwtich->setCurrentIndex(0);
    });
    //  Function: menu action to show model viewport
    connect(ui->actVpModel, &QAction::triggered, this, [&]() {
        ui->vpToolBar->setCurrentIndex(1);
        ui->vpSwtich->setCurrentIndex(1);
    });
    //  Function: menu action to show analysis viewport
    connect(ui->actVpAnalysis, &QAction::triggered, this, [&]() {
        ui->vpToolBar->setCurrentIndex(2);
        ui->vpSwtich->setCurrentIndex(2);
    });
    //  Function: menu action to show optimization viewport
    connect(ui->actVpOpt, &QAction::triggered, this, [&]() {
        ui->vpToolBar->setCurrentIndex(3);
        ui->vpSwtich->setCurrentIndex(3);
    });
    //  Function: menu action to show postprocess viewport
    connect(ui->actVpPost, &QAction::triggered, this, [&]() {
        ui->vpToolBar->setCurrentIndex(4);
        ui->vpSwtich->setCurrentIndex(4);
    });
    //  Function: menu action to show monitor viewport
    connect(ui->actVpMonitor, &QAction::triggered, this, [&]() {
        ui->vpToolBar->setCurrentIndex(5);
        ui->vpSwtich->setCurrentIndex(5);
    });
    //  Function: remote server
    connect(ui->actVpRemote, &QAction::triggered, remote, &QDialog::exec);
}

/*  ############################################################################
 *  setupProject: setup the project related function  */
void pacnano::setupProject() {
    /*  Create Project  */
    project = new Project(this);
    //  new project
    connect(ui->actProjNew, &QAction::triggered, project,
            &Project::createProjectNew);
    connect(ui->btnProjNew, &QPushButton::clicked, project,
            &Project::createProjectNew);
    //  old project
    connect(ui->actProjOld, &QAction::triggered, project,
            &Project::createProjectOld);
    connect(ui->btnProjOld, &QPushButton::clicked, project,
            &Project::createProjectOld);

    //  set the the projectname in Project ComboBox
    ui->projectName->setModel(project->getItemModel());

    //  save project
    connect(ui->btnProjSave, &QToolButton::clicked, project,
            &Project::saveProject);
    connect(ui->actTopProjSave, &QAction::triggered, project,
            &Project::saveProject);

    //  delete project
    connect(ui->btnProjDel, &QToolButton::clicked, project,
            &Project::deleteProject);
    connect(ui->actTopProjDel, &QAction::triggered, project,
            &Project::deleteProject);

    /*  Remote server  */
    remote = new Remote;
    connect(ui->actRemote, &QAction::triggered, remote, &QDialog::show);
    connect(ui->btnProjRemote, &QToolButton::clicked, remote, &QDialog::show);
}

/*  ############################################################################
 *  setupModel: setup the model related environment  */
void pacnano::setupModel() {
    //  create the model object
    model = new Model(this, project->getProjectName(),  //
                      project->getWorkDirectory());
    //  connect the model creation
    connect(ui->actTopModelCreate, &QAction::triggered, this,
            [&]() { model->createModel(); });
    connect(ui->btnCreateModel, &QPushButton::clicked, this,
            [&]() { model->createModel(); });
    //  connect the model manager
    connect(ui->actTopModelMng, &QAction::triggered, this,
            [&]() { model->showManager(); });
    connect(ui->btnModelMng, &QPushButton::clicked, this,
            [&]() { model->showManager(); });

    //  connect the node set creation
    connect(ui->btnSetNode, &QPushButton::clicked, this,
            [&]() { model->createSetNode(); });
    connect(ui->actTopSetNode, &QAction::triggered, this,
            [&]() { model->createSetNode(); });

    //  connect the element set creation
    connect(ui->btnSetElem, &QPushButton::clicked, this,
            [&]() { model->createSetElem(); });
    connect(ui->actTopSetElem, &QAction::triggered, this,
            [&]() { model->createSetElem(); });

    //  connect the set manager
    connect(ui->btnSetManage, &QPushButton::clicked, this,
            [&]() { model->showSetManager(); });
    connect(ui->actTopSetManager, &QAction::triggered, this,
            [&]() { model->showSetManager(); });

    //  connect the model list
    ui->modelList->setModel(model->getModelList());
    connect(ui->modelList, &QComboBox::currentIndexChanged, this,
            [&](int index) { model->setOperatedModel(index); });
}

/*  ############################################################################
 *  setupMaterialDialog: setup the material create dialog  */
void pacnano::setupMaterialCreation() {
    //  create material manager
    material  = new Material;
    matAssign = new MatAssign(this);

    //  Function: create new material
    connect(ui->actTopMatManage, &QAction::triggered, material,
            &Material::manager);
    connect(ui->btnMatManage, &QToolButton::clicked, material,
            &Material::manager);
    //  Function: open the material manager
    connect(ui->btnMatCreate, &QToolButton::clicked, material,
            &Material::create);
    connect(ui->actTopMatCreate, &QAction::triggered, material,
            &Material::create);
    //  Function: assign material
    connect(ui->btnMatAssign, &QPushButton::clicked, matAssign,
            &MatAssign::show);
}

/*  ============================================================================
 *  setupRenderWindow: setup the render window for the model displaying,
 *  such as show the geometry, mesh, rotate the viewport, and zoom the
 *  viewport and so on   */
void pacnano::setupRenderWindow() {
    /*  Create the render window  */
    renWin       = new Viewer(ui->viewWindow);
    QString file = "/home/zhirui.fan/Documents/research/TopOpt-301-1.vtu";
    // QString file = "/Volumes/opt/Documents/code/TopOpt-301-1.vtu";
    Field* field = new Field(file);
    renWin->setInputData(field);

    /*  connect to the camera configuraiton  */
    connect(ui->actConfigCamera, &QAction::triggered, renWin,
            [&]() { renWin->configCamera(); });

    /*  connect to the node selection  */
    connect(ui->actNodeSelect, &QAction::triggered, renWin,
            [&]() { renWin->pickupCells(false); });

    /*  connect to the element selection  */
    connect(ui->actElemSelect, &QAction::triggered, renWin,
            [&]() { renWin->pickupCells(true); });

    /*  connect to show geometry  */
    connect(ui->actShowGeom, &QAction::triggered, renWin, [&]() {
        ui->mainView->setCurrentIndex(1);
        ui->viewWindow->show();
        renWin->showModel();
    });

    /*  connect to show geometry  */
    connect(ui->actShowMesh, &QAction::triggered, renWin,
            [&]() { renWin->showMesh(); });

    /*  show the plane view  */
    connect(ui->actXY, &QAction::triggered, renWin,
            [&]() { renWin->showCameraXY(); });
    connect(ui->actYZ, &QAction::triggered, renWin,
            [&]() { renWin->showCameraYZ(); });
    connect(ui->actXZ, &QAction::triggered, renWin,
            [&]() { renWin->showCameraXZ(); });
    connect(ui->actXYZ, &QAction::triggered, renWin,
            [&]() { renWin->showCameraAxonometric(); });

    /*  hide selection  */
    connect(ui->btnHideCellCancel, &QPushButton::clicked, ui->innerTool, [&]() {
        // ui->innerTool->hide();
        ui->innerTool->setCurrentIndex(0);
        renWin->turnOffPickMode();
        ui->actHideReverse->setDisabled(false);
    });
    connect(ui->btnHideCellOk, &QPushButton::clicked, ui->innerTool, [&]() {
        renWin->hideSelectedCells();
        ui->innerTool->setCurrentIndex(0);
        ui->actHideReverse->setDisabled(false);
    });
    connect(ui->actHideSelect, &QAction::triggered, renWin, [&]() {
        //  get the status of the picker
        if (renWin->isPickerActivated()) {
            renWin->hideSelectedCells();
        } else {
            ui->actHideReverse->setDisabled(true);
            ui->innerTool->show();
            ui->innerTool->setCurrentIndex(1);
            renWin->pickupCells(true);
        }
    });
    /*  extract selection  */
    connect(ui->btnExtractCellCancel, &QPushButton::clicked, ui->innerTool,
            [&]() {
                ui->innerTool->setCurrentIndex(0);
                renWin->turnOffPickMode();
                ui->actHideSelect->setDisabled(false);
            });
    connect(ui->btnExtractCellOk, &QPushButton::clicked, ui->innerTool, [&]() {
        renWin->showSelectedCells();
        ui->innerTool->setCurrentIndex(0);
        ui->actHideSelect->setDisabled(false);
    });
    connect(ui->actHideReverse, &QAction::triggered, renWin, [&]() {
        //  get the status of the picker
        if (renWin->isPickerActivated()) {
            renWin->showSelectedCells();
        } else {
            ui->actHideSelect->setDisabled(true);
            ui->innerTool->setCurrentIndex(2);
            renWin->pickupCells(true);
        }
    });

    /*  show complete model  */
    connect(ui->actShowAll, &QAction::triggered, renWin,
            [&]() { renWin->showCompleteModel(); });
}
