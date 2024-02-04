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
#include "prenano.h"
using namespace PRENANO;

pacnano::pacnano(QWidget* parent) : QMainWindow(parent), ui(new Ui::pacnano) {
    /*  CREATE UI  */
    ui->setupUi(this);

    /*  Open forder  */
    openDir = new Open(this, 0);
    openRst = new Open(this, 8);
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
    ui->innerTool->setCurrentIndex(0);
    isInPostMode = false;

    // Function: change the viewport using ComboBox
    connect(
        ui->vpSwtich, &QComboBox::currentIndexChanged, this, [&](int index) {
            ui->vpToolBar->setCurrentIndex(index);
            isInPostMode = index == 4 ? true : false;
            ui->innerTool->setCurrentIndex(index == 4 ? (isFieldLoad ? 1 : 0)
                                                      : 0);
        });

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
        ui->innerTool->setCurrentIndex(1);
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
    renWin      = new Viewer(ui->viewWindow);
    isFieldLoad = false;

    /*  ************************************************************************
     *  Open results files  */
    connect(ui->btnPostOpen, &QPushButton::clicked, openRst, &QDialog::show);
    connect(ui->actPostOpen, &QAction::triggered, openDir, &QDialog::show);
    connect(openRst, &Open::accepted, this, [&]() {
        //  get the opened file name
        QString rstFile = "";
        openRst->getSelectContent(rstFile);
        fields.append(new Field(rstFile));
        renWin->setInputData(fields.last());
        ui->mainView->setCurrentIndex(1);
        ui->viewWindow->show();
        //  assign the field name list
        ui->fieldName->addItems(fields.last()->getFieldNameList());
        ui->compName->addItems(fields.last()->getCompNameList());
        ui->innerTool->setCurrentIndex(1);
        //  assign the field data to the viewport
        renWin->initPointField(ui->fieldName->currentIndex(),
                               ui->compName->currentIndex(), FIELD_GENERATE);
        renWin->showFieldGeometry();
        isFieldLoad  = true;
        bool* status = renWin->getFieldSwtichStatus();
        ui->fieldName->setEnabled(status[0]);
        ui->compName->setEnabled(status[1]);
    });

    /*  ************************************************************************
     *  post configuration  */
    connect(ui->actPostConfig, &QAction::triggered, renWin,
            [&]() { renWin->configPost(); });
    connect(ui->btnPostConfig, &QPushButton::clicked, renWin,
            [&]() { renWin->configPost(); });

    /*  ************************************************************************
     *  reflect configuration  */
    connect(ui->actReflect, &QAction::triggered, renWin,
            [&]() { renWin->configReflect(); });
    connect(ui->btnReflect, &QPushButton::clicked, renWin,
            [&]() { renWin->configReflect(); });

    /*  ************************************************************************
     *  plane viewer port  */
    connect(ui->actXY, &QAction::triggered, renWin,
            [&]() { renWin->showCameraXY(); });
    connect(ui->actYZ, &QAction::triggered, renWin,
            [&]() { renWin->showCameraYZ(); });
    connect(ui->actXZ, &QAction::triggered, renWin,
            [&]() { renWin->showCameraXZ(); });
    connect(ui->actXYZ, &QAction::triggered, renWin,
            [&]() { renWin->showCameraAxonometric(); });
    /*  connect to the camera configuraiton  */
    connect(ui->actConfigCamera, &QAction::triggered, renWin,
            [&]() { renWin->configCamera(); });

    /*  ************************************************************************
     *  show model and field   */
    //  show model
    connect(ui->actShowGeom, &QAction::triggered, renWin, [&]() {
        ui->viewWindow->show();
        renWin->showModel();
        bool* status = renWin->getFieldSwtichStatus();
        ui->fieldName->setEnabled(status[0]);
        ui->compName->setEnabled(status[1]);
    });
    //  show mesh
    connect(ui->actShowMesh, &QAction::triggered, renWin,
            [&]() { renWin->showMesh(); });
    //  colored field
    connect(ui->actColorMap, &QAction::triggered, renWin, [&]() {
        renWin->showColorField();
        bool* status = renWin->getFieldSwtichStatus();
        ui->fieldName->setEnabled(status[0]);
        ui->compName->setEnabled(status[1]);
    });
    connect(ui->btnColorMap, &QToolButton::clicked, renWin, [&]() {
        renWin->showColorField();
        bool* status = renWin->getFieldSwtichStatus();
        ui->fieldName->setEnabled(status[0]);
        ui->compName->setEnabled(status[1]);
    });
    //  arrow map
    connect(ui->actSymbolMap, &QAction::triggered, renWin, [&]() {
        renWin->showArrowField();
        bool* status = renWin->getFieldSwtichStatus();
        ui->fieldName->setEnabled(status[0]);
        ui->compName->setEnabled(status[1]);
    });
    connect(ui->btnSymbolMap, &QToolButton::clicked, renWin, [&]() {
        renWin->showArrowField();
        bool* status = renWin->getFieldSwtichStatus();
        ui->fieldName->setEnabled(status[0]);
        ui->compName->setEnabled(status[1]);
    });
    //  deformed geometry
    connect(ui->btnDeformGeo, &QPushButton::clicked, renWin, [&]() {
        renWin->showFieldGeometry();
        bool* status = renWin->getFieldSwtichStatus();
        ui->fieldName->setEnabled(status[0]);
        ui->compName->setEnabled(status[1]);
    });
    connect(ui->actDeformGeo, &QAction::triggered, renWin, [&]() {
        renWin->showFieldGeometry();
        bool* status = renWin->getFieldSwtichStatus();
        ui->fieldName->setEnabled(status[0]);
        ui->compName->setEnabled(status[1]);
    });
    //  switch field and its components
    connect(ui->fieldName, &QComboBox::currentIndexChanged, this,
            [&](int index) {
                if (ui->compName->currentIndex() >= 0 && index >= 0) {
                    renWin->initPointField(index, ui->compName->currentIndex(),
                                           FIELD_UPDATE);
                    //  update the field switch status
                    bool* status = renWin->getFieldSwtichStatus();
                    ui->fieldName->setEnabled(status[0]);
                    ui->compName->setEnabled(status[1]);
                }
            });
    connect(ui->compName, &QComboBox::currentIndexChanged, this,
            [&](int index) {
                if (ui->fieldName->currentIndex() >= 0 && index >= 0) {
                    renWin->initPointField(ui->fieldName->currentIndex(), index,
                                           FIELD_UPDATE);
                    //  update the field switch status
                    bool* status = renWin->getFieldSwtichStatus();
                    ui->fieldName->setEnabled(status[0]);
                    ui->compName->setEnabled(status[1]);
                }
            });

    /*  ************************************************************************
     *  connect to selection  */
    connect(ui->actNodeSelect, &QAction::triggered, renWin,
            [&]() { renWin->activePickMode(USE_NODE_MODE); });
    connect(ui->actElemSelect, &QAction::triggered, renWin,
            [&]() { renWin->activePickMode(USE_CELL_MODE); });

    /*  ************************************************************************
     *  hide selection  */
    connect(ui->btnHideCellCancel, &QPushButton::clicked, ui->innerTool, [&]() {
        ui->innerTool->setCurrentIndex(isInPostMode && isFieldLoad ? 1 : 0);
        renWin->turnOffPickMode();
        ui->actExtractSelect->setDisabled(false);
    });
    connect(ui->btnHideCellOk, &QPushButton::clicked, ui->innerTool, [&]() {
        renWin->handleCellPick(USE_HIDE_MODE);
        ui->innerTool->setCurrentIndex(isInPostMode && isFieldLoad ? 1 : 0);
        ui->actExtractSelect->setDisabled(false);
    });
    connect(ui->actHideSelect, &QAction::triggered, renWin, [&]() {
        //  get the status of the picker
        if (renWin->isPickerActivated()) {
            renWin->handleCellPick(USE_HIDE_MODE);
        } else {
            ui->actExtractSelect->setDisabled(true);
            ui->innerTool->show();
            ui->innerTool->setCurrentIndex(2);
            renWin->activePickMode(USE_CELL_MODE);
        }
    });
    connect(ui->btnHideCells, &QToolButton::clicked, renWin, [&]() {
        //  get the status of the picker
        if (renWin->isPickerActivated()) {
            renWin->handleCellPick(USE_HIDE_MODE);
        } else {
            ui->actExtractSelect->setDisabled(true);
            ui->innerTool->show();
            ui->innerTool->setCurrentIndex(2);
            renWin->activePickMode(USE_CELL_MODE);
        }
    });

    /*  ************************************************************************
     *  extract selection   */
    connect(
        ui->btnExtractCellCancel, &QPushButton::clicked, ui->innerTool, [&]() {
            ui->innerTool->setCurrentIndex(isInPostMode && isFieldLoad ? 1 : 0);
            renWin->turnOffPickMode();
            ui->actHideSelect->setDisabled(false);
        });
    connect(ui->btnExtractCellOk, &QPushButton::clicked, ui->innerTool, [&]() {
        renWin->handleCellPick(USE_EXTRACT_MODE);
        ui->innerTool->setCurrentIndex(isInPostMode && isFieldLoad ? 1 : 0);
        ui->actHideSelect->setDisabled(false);
    });
    connect(ui->actExtractSelect, &QAction::triggered, renWin, [&]() {
        //  get the status of the picker
        if (renWin->isPickerActivated()) {
            renWin->handleCellPick(USE_EXTRACT_MODE);
        } else {
            ui->actHideSelect->setDisabled(true);
            ui->innerTool->setCurrentIndex(3);
            renWin->activePickMode(USE_CELL_MODE);
        }
    });
    connect(ui->btnExtractCells, &QToolButton::clicked, renWin, [&]() {
        //  get the status of the picker
        if (renWin->isPickerActivated()) {
            renWin->handleCellPick(USE_EXTRACT_MODE);
        } else {
            ui->actHideSelect->setDisabled(true);
            ui->innerTool->setCurrentIndex(3);
            renWin->activePickMode(USE_CELL_MODE);
        }
    });

    /*  ************************************************************************
     *  complete model  */
    connect(ui->actShowAll, &QAction::triggered, renWin,
            [&]() { renWin->showCompleteModel(); });
    connect(ui->btnCompleteCells, &QToolButton::clicked, renWin,
            [&]() { renWin->showCompleteModel(); });
}
