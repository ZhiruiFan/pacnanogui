/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : viewer.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : October 22th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */

#include "viewer.h"

#include "prenano.h"
using namespace PRENANO;

/*  ############################################################################
 *  Constructor: create the viewer object the initialize the diply
 *  @param  window: the object to the show the FEM model  */
Viewer::Viewer(QVTKOpenGLNativeWidget* window) {
    /*  assign the FEM model viewer */
    win = window;

    /*  camera object  */
    camera = new Camera(nullptr);

    /*  setup the render for FEM model viewer */
    renWin = vtkGenericOpenGLRenderWindow::New();
    render = vtkRenderer::New();
    colors = vtkNamedColors::New();
    actor  = vtkActor::New();
    //  configuration
    render->AddActor(actor);
    renWin->AddRenderer(render);
    win->setRenderWindow(renWin);
    render->SetBackground(colors->GetColor3d("White").GetData());

    /*  field variables  */
    viewMode         = USE_MODEL_MODE;
    dtMap            = vtkDataSetMapper::New();
    isModelLoaded    = false;
    isFieldLoaded    = false;
    isInitViewerPort = true;

    /*  Picker vriables  */
    pick          = Pick::New();
    interact      = renWin->GetInteractor();
    initStyle     = vtkInteractorStyleTrackballCamera::New();
    pickThreshold = vtkThreshold::New();

    /*  coordinates system  */
    axis    = vtkOrientationMarkerWidget::New();
    actaxis = vtkAxesActor::New();
    double rgba[4]{0.0, 0.0, 0.0, 0.0};
    colors->GetColor("Carrot", rgba);
    axis->SetOutlineColor(rgba[0], rgba[1], rgba[2]);
    axis->SetOrientationMarker(actaxis);
    axis->SetViewport(-0.2, -0.1, 0.15, 0.25);
    axis->SetInteractor(renWin->GetInteractor());
    axis->SetEnabled(1);
    axis->InteractiveOff();
    actaxis->GetXAxisShaftProperty()->SetLineWidth(4.0);
    actaxis->GetYAxisShaftProperty()->SetLineWidth(4.0);
    actaxis->GetZAxisShaftProperty()->SetLineWidth(4.0);

    /*  Status bar  */
    status = vtkTextActor::New();
    status->SetTextScaleModeToNone();
    status->GetTextProperty()->SetFontSize(16);
    status->GetTextProperty()->SetFontFamilyToCourier();
    status->GetTextProperty()->SetColor(0.0, 0.0, 0.0);
    status->SetTextScaleModeToNone();
    status->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
    status->GetPositionCoordinate()->SetValue(0.2, 0.1);
    render->AddActor(status);

    /*  arrow viewer  */
    gly        = vtkGlyph3D::New();
    arrow      = vtkArrowSource::New();
    polyMapper = vtkPolyDataMapper::New();

    /*  lookup table and scalar bar  */
    lut               = vtkLookupTable::New();
    scalarBar         = vtkScalarBarActor::New();
    isScalarBarPlayed = false;

    /*  Camera configuration  */
    originCamera = vtkCamera::New();
    originCamera->DeepCopy(render->GetActiveCamera());
    showCameraAxonometric();

    /*  postprocess configuration  */
    post         = new Post(nullptr);
    numIntervals = 12;
    isAutoLegend = false;
    connect(post, &Post::accepted, this, [&]() {
        //  reset the operation flags
        operateType = USE_ORIGIN_FIELD;
        //  assign parameters
        field->setWarpScale(post->getWarpScale());
        field->setLimits(post->getLimitType(), post->getLowerLimit(),
                         post->getUpperLimit());
        numIntervals = post->getNumIntervals();
        isAutoLegend = post->isUseAutoLegend();
        //  update viewerport
        initPointField(recorder[1], recorder[2], FIELD_GENERATE);
    });

    /*  reflect operation  */
    reflect    = new Reflect(this);
    transform  = vtkTransform::New();
    refOperate = vtkTransformFilter::New();
    connect(reflect, &Reflect::accepted, this, [&]() {
        int type = reflect->getOperateType();
        switch (reflect->getOperateType()) {
            case 0:
                initMirrorField();
                break;
        }
    });
}

/*  ============================================================================
 *  destructor: destroy the render object  */
Viewer::~Viewer() {
    //  color and actor
    colors->Delete();
    colors = nullptr;
    actor->Delete();
    actor = nullptr;

    //  vtk render
    render->Delete();
    render = nullptr;

    //  render window
    renWin->Finalize();
    renWin->Delete();
    renWin = nullptr;

    //  the window the show the FEM model
    win = nullptr;
}

/*  ############################################################################
 *  setInputData: set the input filed data that will be shown in the render
 *      window.
 *  @param  input: the filed varaible  */
void Viewer::setInputData(Field*& input) {
    /*  set the field variable that will be shown  */
    field = input;
    pick->setField(input);

    /*  copy the current unstructured data for operation  */
    portModelCur  = field->getInputPort();
    portFieldCur  = field->getThresholdOutputPort();
    ugridFieldCur = field->getInputData();

    /*  update the flag  */
    isModelLoaded = true;
    operateType   = USE_ORIGIN_FIELD;

    /*  initialize the filed  */
    field->checkAnchor();
};

/*  ############################################################################
 *  update: update the displayed object using the current port  */
void Viewer::update() {
    /*  show the field data as current configuration  */
    switch (recorder[0]) {
        /*   show model   */
        case 0: {
            showModel();
            break;
        }
        /*  show point related field  */
        case 1: {
            showColorField();
            break;
        }
        /*  show arrow field  */
        case 2: {
            showArrowField();
            break;
        }
        /*  show field geometry  */
        case 3: {
            showFieldGeometry();
            break;
        }
    }
}

/*  ############################################################################
 *  showCompleteModel: reset the unstructured grid data to the original and
 *      show the model as current configuration  */
void Viewer::showCompleteModel() {
    /*  check the status  */
    if (isModelLoaded) {
        /*  reset the unstructured grid to original  */
        field->resetCellPick(operateType);
        portModelCur  = field->getInputPort();
        ugridFieldCur = field->getInputData();
        /*  update the anchor in field  */
        field->updateAnchor();
        /*  update the field data port  */
        switch (operateType) {
            //  use the original field
            case USE_ORIGIN_FIELD:
                portFieldCur = field->getThresholdOutputPort();
                break;
            //  use mirrored field
            case USE_MIRROR_FIELD:
                portFieldCur = field->getMirrorOutputPort();
                break;
        }
        /*  update the display  */
        update();
    }
}

/*  ############################################################################
 *  showModel: display the model geometry in the viewer object
 *  @param  index: the index of the model that will be shown  */
void Viewer::showModel() {
    /*  check the field is assigned or not  */
    if (isModelLoaded) {
        /*  assign the flags  */
        recorder[0] = 0;
        viewMode    = USE_MODEL_MODE;

        /*  reset the interactor style  */
        pick->turnOff();
        interact->SetInteractorStyle(initStyle);

        /*  set the head information  */
        QString info = "Geometry of the current model";
        configStatusBar(field->getPathName(), info);

        /*  remove the scalar bar  */
        if (isScalarBarPlayed) {
            render->RemoveActor(scalarBar);
            isScalarBarPlayed = false;
        }

        /*  set the data to the viewer  */
        dtMap->SetInputConnection(portModelCur);
        dtMap->ScalarVisibilityOff();

        /*  configure the actor  */
        actor->GetProperty()->SetColor(colors->GetColor3d("cyan").GetData());
        actor->GetProperty()->SetEdgeVisibility(0);
        actor->SetMapper(dtMap);

        /*  show mesh and config camera  */
        /*  reset the camera  */
        if (isInitViewerPort) {
            isInitViewerPort = false;
            render->ResetCamera();
        }
        renWin->Render();

        /*  update the model loaded flag  */
        isModelLoaded = true;
    }
}

/*  ############################################################################
 *  showMesh: display the FEM model in the viewer object
 *  @param  file: the file to read the model mesh information  */
void Viewer::showMesh() {
    /*  check the status of the model  */
    if (isModelLoaded || isFieldLoaded) {
        /*  assign the recorder  */
        recorder[0] = 1;

        /*  configure the actor  */
        bool isOn = actor->GetProperty()->GetEdgeVisibility();
        actor->GetProperty()->SetEdgeVisibility(!isOn);
        actor->GetProperty()->SetLineWidth(0.0);

        /*  show mesh and config camera  */
        renWin->Render();
    }
}

/*  ############################################################################
 *  initPointField: initialize the specified point field from the original
 *  vtu files
 *  @param  idx: the index of point data that will be shown
 *  @param  comp: the component of the point data that will be extracted
 *  @param  mode: update mode or generate mode  */
void Viewer::initPointField(const int& idx, const int& comp, const bool& mode) {
    /*  update the flags  */
    isFieldLoaded = true;

    /*  assign the recorder  */
    recorder[1] = idx;
    recorder[2] = comp;

    /*  regenerate the field variable if needed  */
    if (mode == FIELD_GENERATE) {
        //  update the field variables
        field->updateAnchor();
        ugridFieldCur = field->getThresholdOutput();
        portFieldCur  = field->getThresholdOutputPort();
        //  reset the viewer port
        render->ResetCamera();
    }

    /*  Create the LOOKUP table  */
    std::stringstream name;
    name << field->getFieldName(idx) << ":" << field->getCompName(comp);
    std::string arrayName;
    arrayName.assign(name.str());
    lut->SetHueRange(0.667, 0.0);
    lut->SetTableRange(field->getThresholdRange(name.str().data()));
    lut->Build();

    /*  update the viewer port  */
    update();
}

/*  ============================================================================
 *  showPointField: display the information with respect to the nodes,
 *  it includes the nodal displacement, reaction force and so on
 *  @param  field: the field that to be shown
 *  @param  idx: the index of the component in the data set
 *  @param  comp: the component index  */
void Viewer::showColorField() {
    /*  check the if the model is loaded  */
    if (isFieldLoaded) {
        /*  update the model or field flag  */
        viewMode = USE_FIELD_MODE;

        /*  assign the recorder */
        recorder[0] = 1;

        /*  get the field index and its component  */
        int index = recorder[1];
        int comp  = recorder[2];

        /*  get the name of the field  */
        std::stringstream name;
        name << field->getFieldName(index) << ":" << field->getCompName(comp);
        std::string fieldName = name.str();

        /*  Create the LOOKUP table  */
        //  update the range of field data
        if (isAutoLegend) {
            lut->SetNumberOfTableValues(numIntervals);
            lut->SetTableRange(ugridFieldCur->GetPointData()
                                   ->GetArray(name.str().c_str())
                                   ->GetRange());
        }
        //  update the lookup table
        lut->Build();

        /*  configure the scalar bar  */
        configScalarBar();
        scalarBar->SetLookupTable(lut);
        scalarBar->SetTitle(name.str().c_str());
        render->AddActor(scalarBar);
        isScalarBarPlayed = true;

        /*  setup the mapper  */
        dtMap->RemoveAllInputConnections(0);
        dtMap->SetInputConnection(0, portFieldCur);

        /*  set the anchor of the warpper  */
        dtMap->SetLookupTable(lut);
        dtMap->ScalarVisibilityOn();
        dtMap->SetScalarModeToUsePointFieldData();
        dtMap->SelectColorArray(name.str().c_str());
        dtMap->SetScalarRange(lut->GetRange());

        /*  Setup the actor  */
        actor->SetMapper(dtMap);
        render->AddActor2D(scalarBar);

        /*  Render the window  */
        renWin->Render();
    }
}

/*  ============================================================================
 *  showArrowField: show the data using the arrow  */
void Viewer::showArrowField() {
    /*  check the field is loaded or not  */
    if (isFieldLoaded) {
        /*  update the model or field flag  */
        viewMode = USE_FIELD_MODE;

        /*  assign the recorder */
        recorder[0] = 2;

        /*  get the field index and its component  */
        int index = recorder[1];
        int comp  = recorder[2];

        /*  get the name of the field  */
        std::stringstream name;
        name << field->getPointDataArrayName(index) << ":" << compName[comp];

        /*  Create the LOOKUP table  */
        //  update the range of field data
        if (isAutoLegend) {
            lut->SetNumberOfTableValues(numIntervals);
            lut->SetTableRange(ugridFieldCur->GetPointData()
                                   ->GetArray(name.str().c_str())
                                   ->GetRange());
        }
        //  update the lookup table
        lut->Build();

        /*  setup the mapper  */
        gly->SetInputConnection(portFieldCur);
        gly->SetSourceConnection(arrow->GetOutputPort());
        gly->SetScaleFactor(100.0);
        gly->SetVectorModeToUseVector();
        gly->SetInputArrayToProcess(0, 0, 0,
                                    vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                    field->getPointDataArrayName(index));
        polyMapper->RemoveAllInputConnections(0);
        polyMapper->AddInputConnection(0, gly->GetOutputPort());
        polyMapper->SetLookupTable(lut);
        polyMapper->ScalarVisibilityOn();
        polyMapper->SetScalarModeToUsePointFieldData();
        polyMapper->SelectColorArray(name.str().c_str());
        polyMapper->SetScalarRange(lut->GetRange());
        actor->SetMapper(polyMapper);

        /*  configuration of the scalar bar  */
        configScalarBar();
        scalarBar->SetLookupTable(lut);
        scalarBar->SetTitle(name.str().c_str());
        render->AddActor(scalarBar);
        isScalarBarPlayed = true;

        /*  Render the window  */
        renWin->Render();
    }
}

/*  ============================================================================
 *  showFieldGeometry: display the information with respect to the nodes,
 *  it includes the nodal displacement, reaction force and so on  */
void Viewer::showFieldGeometry() {
    /*  check if  the field is loaded  */
    if (isFieldLoaded) {
        /*  assign the recorder */
        recorder[0] = 3;

        /*  assign the flags  */
        viewMode = USE_FIELD_MODE;

        /*  set the head information  */
        QString info = "Display the geometry of the current model";
        configStatusBar(field->getPathName(), info);

        /*  remove the scalar bar  */
        if (isScalarBarPlayed) {
            render->RemoveActor(scalarBar);
            isScalarBarPlayed = false;
        }

        /*  set the data to the viewer  */
        // dtMap->SetInputData(ugridCur);
        dtMap->SetInputConnection(portFieldCur);
        dtMap->ScalarVisibilityOff();

        /*  configure the actor  */
        actor->GetProperty()->SetColor(colors->GetColor3d("cyan").GetData());
        actor->SetMapper(dtMap);

        /*  show mesh and config camera  */
        renWin->Render();
    }
}

/*  ============================================================================
 *  initMirrorField: show the reflected field according to the specified
 *  parameters  */
void Viewer::initMirrorField() {
    /*  generate the mirror field  */
    field->mirror(reflect->isUseFullModel(), reflect->getMirrorPlane(),
                  reflect->getCoordsOfOrigin(), reflect->getRotation());

    /*  update the field information in viewer port  */
    if (isFieldLoaded) {
        /*  update the model or field flag  */
        viewMode    = USE_FIELD_MODE;
        operateType = USE_MIRROR_FIELD;

        /*  define the data and port in current  */
        ugridFieldCur = field->getMirrorOutput();
        portFieldCur  = field->getMirrorOutputPort();

        /*  update the viewerport  */
        update();
    }
}

/*  ############################################################################
 *  showCellField: display the information with respect to the elements,
 *  it includes the stress components, design variables in topology
 *  optimization and so on
 *  @param  idx: the index of the component in the data set  */
void Viewer::showCellField(const int& idx) {}

/*  ############################################################################
 *  pickCells: pick up the cells in the viewerport using the mouse box
 *  selection method.
 *  @param  field: the field that will be shown
 *  @param  mode: node or element selection mode, if true, then node selection
 *                mode; else element selection mode  */
void Viewer::activePickMode(const bool& mode) {
    /*  check the model is loaded or not  */
    if (isModelLoaded) {
        /*  create the picker  */
        if (viewMode == USE_MODEL_MODE) {
            pick->setSourcePort(field->getInputPort());
            pick->setInputData(portModelCur);
        } else {
            /*  determine the source field for operation  */
            switch (operateType) {
                //  the initial field source
                case USE_ORIGIN_FIELD:
                    pick->setSourcePort(field->getThresholdOutputPort());
                    break;
                //  the mirrored field source
                case USE_MIRROR_FIELD:
                    pick->setSourcePort(field->getMirrorOutputPort());
                    break;
            }
            pick->setInputData(portFieldCur);
        }
        pick->setRenderInfo(render);

        /*  determine the selection mode */
        if (mode == USE_CELL_MODE) {
            pick->setCellSelectMode();
        } else {
            pick->setPointSelectMode();
        }
        //  set the picker to the interactor
        interact->SetInteractorStyle(pick);
        pick->OnLeftButtonUp();
    }
}

/*  ============================================================================
 *  hideCells: hide the cells selected by the picker  */
void Viewer::handleCellPick(const bool& pickMode) {
    /*  Get the picker status  */
    if (pick->isPickerActivated() && pick->isCellSelectionModeOn()) {
        /*  get the current selected cells  */
        cellIdsCur = pick->getSelectedCellIds();

        /*  update the current port  */
        field->performCellPick(operateType, viewMode, pickMode, cellIdsCur);
        if (viewMode == USE_MODEL_MODE) {
            portModelCur = field->getPickOutputPort();
        } else {
            portFieldCur  = field->getPickOutputPort();
            ugridFieldCur = field->getPickOutput();
        }

        /*  turn off picker  */
        pick->turnOff();
        interact->SetInteractorStyle(initStyle);

        /*  render the window  */
        update();
    }
}

/*  ############################################################################
 *  showCamera: configure the camera to show the axionometric view  */
void Viewer::showCameraAxonometric() {
    /*  reset to camera to the original  */
    render->GetActiveCamera()->DeepCopy(originCamera);
    render->GetActiveCamera()->Elevation(45.0);
    render->GetActiveCamera()->Azimuth(45.0);
    render->GetActiveCamera()->Dolly(1.0);
    render->ResetCamera();
    renWin->Render();
}

/*  ============================================================================
 *  showCameraXY: configure the camera to show the XY plane view  */
void Viewer::showCameraXY() {
    /*  reset to camera to the original  */
    render->GetActiveCamera()->DeepCopy(originCamera);
    /*  set the camera configuration  */
    render->GetActiveCamera()->Elevation(0.0);
    render->GetActiveCamera()->Azimuth(0.0);
    render->GetActiveCamera()->Dolly(1.0);
    render->GetActiveCamera()->ParallelProjectionOn();
    render->ResetCamera();
    renWin->Render();
}

/*  ============================================================================
 *  showCameraXZ: configure the camera to show XZ plane view  */
void Viewer::showCameraXZ() {
    /*  reset to camera to the original  */
    render->GetActiveCamera()->DeepCopy(originCamera);
    /*  set the camera configuration  */
    render->GetActiveCamera()->Elevation(-90.0);
    render->GetActiveCamera()->Azimuth(90.0);
    render->GetActiveCamera()->SetViewUp(0, 0, 1);
    render->GetActiveCamera()->ParallelProjectionOn();
    render->ResetCamera();
    renWin->Render();
}

/*  ============================================================================
 *  showCameraYZ: configure the camera to show the YZ plane view  */
void Viewer::showCameraYZ() {
    /*  reset to camera to the original  */
    render->GetActiveCamera()->DeepCopy(originCamera);

    /*  set the camera configuration  */
    render->GetActiveCamera()->Elevation(0.0);
    render->GetActiveCamera()->Azimuth(-90.0);
    render->GetActiveCamera()->ParallelProjectionOn();
    render->ResetCamera();
    renWin->Render();
}

/*  ############################################################################
 *  configure the status bar, which includes the system time, model
 *  information
 *  @param  file: the model that will be displayed
 *  @param  info: the model information that will be shown  */
void Viewer::configStatusBar(QString& file, QString& info) {
    /*  Get the system time  */
    auto curTime            = std::chrono::system_clock::now();
    std::time_t curTime_t   = std::chrono::system_clock::to_time_t(curTime);
    struct std::tm* locTime = std::localtime(&curTime_t);
    time.clear();
    time.str("");
    time << locTime->tm_year + 1900 << "-" << locTime->tm_mon + 1 << "-"
         << locTime->tm_mday << ":" << locTime->tm_hour << ":"
         << locTime->tm_min << ":" << locTime->tm_sec;

    /*  configure the status information  */
    QString data = "File: " + file +  //
                   "\nDescription: " + info + "\n\nDate: ";
    data.append(time.str());
    status->SetInput(data.toStdString().c_str());
}

/*  ============================================================================
 *  showScalarBar: configure the scalar bar style and then display it  */
void Viewer::configScalarBar() {
    /*  turn off the auto font setting */
    scalarBar->UnconstrainedFontSizeOn();

    /*  set the position  */
    int* renSize      = renWin->GetSize();
    int legendHight   = 17 * numIntervals;
    double leftMargin = 45.0 / renSize[0];
    double topMargin  = 1.0 - (legendHight + 50.0) / renSize[1];
    scalarBar->SetPosition(leftMargin, topMargin);
    scalarBar->SetMaximumWidthInPixels(70);
    scalarBar->SetMaximumHeightInPixels(legendHight);

    /*  label data format  */
    scalarBar->SetLabelFormat("%+.4e");

    /*  set the label style  */
    scalarBar->GetLabelTextProperty()->SetColor(0.0, 0.0, 0.0);
    scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();
    scalarBar->GetLabelTextProperty()->SetFontSize(15);
    scalarBar->GetLabelTextProperty()->SetBold(0);
    scalarBar->GetLabelTextProperty()->SetItalic(0);
    scalarBar->GetLabelTextProperty()->SetJustificationToLeft();
    scalarBar->GetLabelTextProperty()->SetVerticalJustificationToBottom();
    scalarBar->SetNumberOfLabels(numIntervals);

    /*  set the title style  */
    scalarBar->GetTitleTextProperty()->SetColor(0.0, 0.0, 0.0);
    scalarBar->GetTitleTextProperty()->SetFontSize(18);
    scalarBar->GetTitleTextProperty()->SetJustificationToLeft();
    scalarBar->GetTitleTextProperty()->SetVerticalJustificationToTop();
    scalarBar->GetTitleTextProperty()->SetFontFamilyToCourier();
    scalarBar->GetTitleTextProperty()->SetBold(0);
    scalarBar->GetTitleTextProperty()->SetItalic(0);
}
