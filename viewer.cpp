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

#include <vtkInformationKey.h>

/*  ############################################################################
 *  Constructor: create the viewer object the initialize the diply
 *  @param  window: the object to the show the FEM model  */
Viewer::Viewer(QVTKOpenGLNativeWidget* window) {
    /*  assign the FEM model viewer */
    win = window;

    /*  setup the render for FEM model viewer */
    renWin = vtkGenericOpenGLRenderWindow::New();
    render = vtkRenderer::New();
    renWin->AddRenderer(render);
    win->setRenderWindow(renWin);
    colors = vtkNamedColors::New();
    render->SetBackground(colors->GetColor3d("White").GetData());

    /*  create the data mapper  */
    dtMap = vtkDataSetMapper::New();

    /*  create the actor  */
    actor = vtkActor::New();
    render->AddActor(actor);

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
    status->GetTextProperty()->SetFontSize(15);
    status->GetTextProperty()->SetFontFamilyToCourier();
    status->GetTextProperty()->SetColor(0.0, 0.0, 0.0);
    status->SetTextScaleModeToNone();
    status->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
    status->GetPositionCoordinate()->SetValue(0.2, 0.1);
    render->AddActor(status);

    /*  lookup table  */
    lut = vtkLookupTable::New();

    /*  Scalar bar  */
    scalarBar         = vtkScalarBarActor::New();
    isScalarBarPlayed = false;
    configScalarBar();

    /*  Picker  */
    cellPicker = vtkCellPicker::New();
    cellPicker->SetTolerance(0.01);

    /*  show the model */
    QString file = "/home/zhirui.fan/Documents/research/TopOpt-301-1.vtu";
    Field* field = new Field(file);
    //    showModel(file);
    //    showMesh(field);
    //    std::string name = "U";
    //    int comp         = 0;
    //    showPointField(field, 0, 0);
    pickupCells(field);
}

/*  ----------------------------------------------------------------------------
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
 *  showModel: display the model geometry in the viewer object
 *  @param  index: the index of the model that will be shown  */
void Viewer::showModel(Field*& field) {
    /*  set the head information  */
    QString info = "Geometry of the current model";
    configStatusBar(field->name, info);

    /*  set the data to the viewer  */
    dtMap->SetInputConnection(field->port);
    dtMap->ScalarVisibilityOff();

    /*  configure the actor  */
    actor->GetProperty()->SetColor(colors->GetColor3d("cyan").GetData());
    actor->GetProperty()->SetEdgeVisibility(0);
    actor->GetProperty()->SetLineWidth(2.0);
    actor->SetMapper(dtMap);

    /*  configure the camera  */
    configCamera();
}

/*  ----------------------------------------------------------------------------
 *  showMesh: display the FEM model in the viewer object
 *  @param  file: the file to read the model mesh information  */
void Viewer::showMesh(Field*& field) {
    /*  set the head information  */
    QString info = "Geometry of the current model";
    configStatusBar(field->name, info);

    /*  set the data to the viewer  */
    dtMap->SetInputConnection(field->port);
    dtMap->ScalarVisibilityOff();

    /*  configure the actor  */
    actor->GetProperty()->SetColor(colors->GetColor3d("cyan").GetData());
    actor->GetProperty()->SetEdgeVisibility(1);
    actor->GetProperty()->SetLineWidth(0.0);
    actor->SetMapper(dtMap);

    /*  configure the camera  */
    configCamera();
}

/*  ############################################################################
 *  showPointField: display the information with respect to the nodes,
 *  it includes the nodal displacement, reaction force and so on
 *  @param  field: the field that to be shown
 *  @param  idx: the index of the component in the data set
 *  @param  comp: the component index  */
void Viewer::showPointField(Field*& field, const int& index, const int& comp) {
    /*  Get the the field  */
    vtkDataArray* dtOld = field->pointData->GetArray(index);

    /*  Create the temporary variable to show field  */
    vtkDoubleArray* dtCur = vtkDoubleArray::New();
    dtCur->SetNumberOfComponents(1);

    /*  Extract the components in the field  */
    std::stringstream name;
    if (comp < 3) {
        //  extract the sub components
        for (vtkIdType i = 0; i < dtOld->GetNumberOfTuples(); ++i) {
            dtCur->InsertNextValue(dtOld->GetTuple(i)[comp]);
        }
        //  determine the name of the components
        name << dtOld->GetName() << ":" << compName[comp];
    } else {
        // extract the amplitude of the field
        double x, y, z;
        for (vtkIdType i = 0; i < dtOld->GetNumberOfTuples(); ++i) {
            x = dtOld->GetTuple(i)[0];
            y = dtOld->GetTuple(i)[1];
            z = dtOld->GetTuple(i)[2];
            dtCur->InsertNextValue(sqrt(x * x + y * y + z * z));
        }
        //  determine the name of the components
        name << dtOld->GetName() << ": total";
    }
    //  assign the name of the components
    dtCur->SetName(name.str().c_str());
    //  set the current displayed data
    field->pointData->SetScalars(dtCur);

    /*  Create the LOOKUP table  */
    lut->SetHueRange(0.667, 0.0);
    lut->SetNumberOfTableValues(12);
    lut->SetTableRange(dtCur->GetRange());
    lut->Build();

    /*  Create the scalar bar  */
    if (!isScalarBarPlayed) {
        scalarBar->SetLookupTable(lut);
        scalarBar->SetTitle(name.str().c_str());
        render->AddActor(scalarBar);
        isScalarBarPlayed = true;
    }

    /*  Update the warpper  */
    field->updateWarper();

    /*  Config the status bar  */
    QString info = "Plot the calculated field of the finite element model.";
    configStatusBar(field->name, info);

    /*  setup the mapper  */
    dtMap->SetInputConnection(field->warp->GetOutputPort());
    dtMap->SetScalarVisibility(1);
    dtMap->SetScalarModeToUsePointData();
    dtMap->SetScalarRange(dtCur->GetRange());
    dtMap->SetLookupTable(lut);
    dtMap->SelectColorArray(dtCur->GetName());

    /*  Setup the actor  */
    actor->SetMapper(dtMap);
    actor->GetProperty()->SetEdgeVisibility(field->ifMeshed);
    actor->GetProperty()->SetLineWidth(0.0);
    render->AddActor2D(scalarBar);

    /*  Configure the camera  */
    configCamera();
    renWin->Render();
}

/*  ============================================================================
 *  configCamera: configure the camera to show the FEM model appropriately  */
void Viewer::configCamera() {
    /*  set the camera configuration  */
    render->ResetCamera();
    render->GetActiveCamera()->Elevation(60.0);
    render->GetActiveCamera()->Azimuth(30.0);
    render->GetActiveCamera()->Dolly(1.2);
    render->GetActiveCamera()->SetClippingRange(
        0.1, std::numeric_limits<double>::max());
}

/*  ============================================================================
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
 *  configScalarBar: configure the scalar bar style  */
void Viewer::configScalarBar() {
    /*  turn off the auto font setting */
    scalarBar->UnconstrainedFontSizeOn();

    /*  set the title style  */
    scalarBar->GetTitleTextProperty()->SetColor(0.0, 0.0, 0.0);
    scalarBar->GetTitleTextProperty()->SetFontSize(20);
    scalarBar->GetTitleTextProperty()->SetJustificationToLeft();
    scalarBar->GetTitleTextProperty()->SetVerticalJustificationToTop();
    scalarBar->GetTitleTextProperty()->SetFontFamilyToCourier();
    scalarBar->GetTitleTextProperty()->SetBold(0);
    scalarBar->GetTitleTextProperty()->SetItalic(0);

    /*  set the label style  */
    scalarBar->GetLabelTextProperty()->SetColor(0.0, 0.0, 0.0);
    scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();
    scalarBar->GetLabelTextProperty()->SetFontSize(15);
    scalarBar->GetLabelTextProperty()->SetBold(0);
    scalarBar->GetLabelTextProperty()->SetItalic(0);
    scalarBar->GetLabelTextProperty()->SetJustificationToLeft();
    scalarBar->GetLabelTextProperty()->SetVerticalJustificationToBottom();
    scalarBar->SetNumberOfLabels(12);

    /*  set the position  */
    scalarBar->SetPosition(0.02, 0.60);
    scalarBar->SetMaximumWidthInPixels(70);
    scalarBar->SetMaximumHeightInPixels(200);

    /*  label data format  */
    scalarBar->SetLabelFormat("%.4e");
}

/*  ############################################################################
 *  pickCells: pick up the cells in the viewerport using the mouse box
 *  selection method.  */
void Viewer::pickupCells(Field* field) {
    /*  Create the Poly data from the field  */
    vtkGeometryFilter* geom = vtkGeometryFilter::New();
    geom->SetInputData(field->ugrid);
    geom->Update();
    vtkPolyData* polyData = geom->GetOutput();

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkIdFilter> idFilter;
    idFilter->SetInputData(polyData);
    idFilter->SetCellIdsArrayName("OriginalIds");
    idFilter->SetPointIdsArrayName("OriginalIds");
    idFilter->Update();

    // This is needed to convert the ouput of vtkIdFilter (vtkDataSet) back to
    // vtkPolyData.
    vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
    surfaceFilter->Update();

    vtkPolyData* input = surfaceFilter->GetOutput();

    // Create a mapper and actor.
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(5);
    actor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Peacock").GetData());
    // Visualize
    vtkNew<vtkRenderer> renderer;
    renderer->UseHiddenLineRemovalOn();

    //    QVTKOpenGLNativeWidget* window;
    //    window = new QVTKOpenGLNativeWidget;
    vtkNew<vtkRenderWindow> renderWindow;
    win->setRenderWindow(renderWindow);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("HighlightSelection");

    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("Tan").GetData());

    renderWindow->Render();

    vtkNew<Pick> style;
    style->setPolyData(input);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindowInteractor->Start();
}

/*  ============================================================================
 *  New: define the New function using the built-in interface of VTK  */
vtkStandardNewMacro(Viewer::Pick);

/*  ============================================================================
 *  OnLectButtonUp: the overrided member function to define the event when the \
 *  left mouse button is up  */
void Viewer::Pick::OnLeftButtonUp() {
    /*  perform the forward member function  */
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

    /*  if the selection mode is actived  */
    if (CurrentMode == 1) {
        vtkNew<vtkNamedColors> colors;

        vtkPlanes* frustum =
            static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())
                ->GetFrustum();

        vtkNew<vtkExtractPolyDataGeometry> extractPolyDataGeometry;
        extractPolyDataGeometry->SetInputData(polyData);
        extractPolyDataGeometry->SetImplicitFunction(frustum);
        extractPolyDataGeometry->Update();

        std::cout << "Extracted "
                  << extractPolyDataGeometry->GetOutput()->GetNumberOfCells()
                  << " cells." << std::endl;
        this->selectMap->SetInputData(extractPolyDataGeometry->GetOutput());
        this->selectMap->ScalarVisibilityOff();

        // vtkIdTypeArray* ids =
        // dynamic_cast<vtkIdTypeArray*>(selected->GetPointData()->GetArray("OriginalIds"));

        this->selectActor->GetProperty()->SetColor(
            colors->GetColor3d("Tomato").GetData());
        this->selectActor->GetProperty()->SetPointSize(5);
        this->selectActor->GetProperty()->SetRepresentationToWireframe();

        this->GetInteractor()
            ->GetRenderWindow()
            ->GetRenderers()
            ->GetFirstRenderer()
            ->AddActor(selectActor);
        this->GetInteractor()->GetRenderWindow()->Render();
        this->HighlightProp(NULL);
    }
}
