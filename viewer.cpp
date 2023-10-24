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

    /*  create the data points  */

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
    lut       = vtkLookupTable::New();
    scalarBar = vtkScalarBarActor::New();
    configScalarBar();

    /*  show the model */
    std::string file = "F:\\code\\TopOpt-301-1.vtu";
    //    showModel(file);
    int index = 0;
    int comp  = 0;
    showMesh(index);
    std::string name = "U";
    showField(index, comp);
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
void Viewer::showModel(int& index) {
    /*  get the model object according to the index  */
    Model* model;
    for (QList<Model*>::iterator it = modelList.begin(); it != modelList.end();
         ++it) {
        if ((*it)->index == index) {
            model = *it;
        }
    }
    /*  set the head information  */
    std::string info = "Geometry of the current model";
    configStatusBar(model->file, info);

    /*  set the data to the viewer  */
    dtMap->SetInputConnection(model->port);
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
void Viewer::showMesh(int& index) {
    /*  get the model object according to the index  */
    Model* model;
    for (QList<Model*>::iterator it = modelList.begin(); it != modelList.end();
         ++it) {
        if ((*it)->index == index) {
            model = *it;
        }
    }
    /*  set the head information  */
    std::string info = "Geometry of the current model";
    configStatusBar(model->file, info);

    /*  set the data to the viewer  */
    dtMap->SetInputConnection(model->port);
    dtMap->ScalarVisibilityOff();

    /*  configure the actor  */
    actor->GetProperty()->SetColor(colors->GetColor3d("cyan").GetData());
    actor->GetProperty()->SetEdgeVisibility(0);
    actor->GetProperty()->SetLineWidth(2.0);
    actor->SetMapper(dtMap);

    /*  configure the camera  */
    configCamera();
}

/*  ############################################################################
 *  extract the point and cell data  */
void Viewer::showField(int& index, int& comp) {
    /*  get the current model  */
    Model* model;
    for (QList<Model*>::iterator it = modelList.begin(); it != modelList.end();
         ++it) {
        if ((*it)->index == index) {
            model = *it;
        }
    }

    //  extract the x-displacement
    dtOld = vtkDoubleArray::SafeDownCast(model->pointData->GetArray(comp));
    if (!dtCur) dtCur->Delete();
    dtCur = vtkDoubleArray::New();
    std::stringstream name;
    name << dtOld->GetName() << "_" << compName[comp];
    dtCur->SetName(name.str().c_str());
    dtCur->SetNumberOfComponents(1);
    for (vtkIdType i = 0; i < dtOld->GetNumberOfTuples(); ++i) {
        dtCur->InsertNextValue(dtOld->GetTuple(i)[comp]);
    }

    /*  set the current display object  */
    model->pointData->SetScalars(dtCur);

    //  create the color lookup table
    lut->SetHueRange(0.667, 0.0);
    lut->SetNumberOfTableValues(10);
    lut->SetTableRange(dtCur->GetRange());
    lut->Build();

    //  create the scalar bar
    scalarBar->SetLookupTable(lut);
    scalarBar->SetTitle("UX");

    //  setup the mapper
    dtMap->SetInputData(model->ugrid);
    dtMap->SetScalarModeToUsePointData();
    dtMap->SetScalarRange(dtCur->GetRange());
    dtMap->SetLookupTable(lut);
    dtMap->SelectColorArray(dtCur->GetName());
    actor->SetMapper(dtMap);
    //    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetLineWidth(0.0);
    //    render->AddActor(actor);
    render->AddActor2D(scalarBar);
}

/*  ----------------------------------------------------------------------------
 *  configCamera: configure the camera and show the FEM model appropriately  */
void Viewer::configCamera() {
    /*  set the camera configuration  */
    render->ResetCamera();
    render->GetActiveCamera()->Elevation(60.0);
    render->GetActiveCamera()->Azimuth(30.0);
    render->GetActiveCamera()->Dolly(1.2);
}

/*  ----------------------------------------------------------------------------
 *  configure the status bar, which includes the system time, model
 *  information
 *  @param  file: the model that will be displayed
 *  @param  info: the model information that will be shown  */
void Viewer::configStatusBar(std::string& file, std::string& info) {
    /*  Get the system time  */
    auto curTime            = std::chrono::system_clock::now();
    std::time_t curTime_t   = std::chrono::system_clock::to_time_t(curTime);
    struct std::tm* locTime = std::localtime(&curTime_t);
    time.clear();
    time << locTime->tm_year + 1900 << "-" << locTime->tm_mon + 1 << "-"
         << locTime->tm_mday << ":" << locTime->tm_hour << ":"
         << locTime->tm_min << ":" << locTime->tm_sec;

    /*  configure the status information  */
    std::string data = "File: " + file +           //
                       "\nDescription: " + info +  //
                       "\n\nDate: " + time.str();
    status->SetInput(data.c_str());
}

/*  ----------------------------------------------------------------------------
 *  configScalarBar: configure the scalar bar style  */
void Viewer::configScalarBar() {
    //  turn off the auto font setting
    scalarBar->UnconstrainedFontSizeOn();

    //  set the title
    scalarBar->GetTitleTextProperty()->SetColor(0.0, 0.0, 0.0);
    scalarBar->GetTitleTextProperty()->SetFontSize(20);
    scalarBar->GetTitleTextProperty()->SetJustificationToLeft();
    scalarBar->GetTitleTextProperty()->SetVerticalJustificationToTop();
    scalarBar->GetTitleTextProperty()->SetFontFamilyToCourier();
    scalarBar->GetTitleTextProperty()->SetBold(0);
    scalarBar->GetTitleTextProperty()->SetItalic(0);

    //  set the font type and size
    scalarBar->GetLabelTextProperty()->SetColor(0.0, 0.0, 0.0);
    scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();
    scalarBar->GetLabelTextProperty()->SetFontSize(15);
    scalarBar->GetLabelTextProperty()->SetBold(0);
    scalarBar->GetLabelTextProperty()->SetItalic(0);
    scalarBar->GetLabelTextProperty()->SetJustificationToLeft();
    scalarBar->GetLabelTextProperty()->SetVerticalJustificationToBottom();

    //  set the number of labels in scalar bar
    scalarBar->SetNumberOfLabels(10);

    //  set the position of the scalar bar
    scalarBar->SetPosition(0.04, 0.60);
    scalarBar->SetMaximumWidthInPixels(70);
    scalarBar->SetMaximumHeightInPixels(200);

    //  label data format
    scalarBar->SetLabelFormat("%.4e");
}

/*  ############################################################################
 *  nested class: Model
 *  Description: the class to read and configure the field variables of each
 *  data files, which includes the point data and cell data
 *  constructor: create a field objective
 *  @param  file: the path to read the vtu file  */
Viewer::Model::Model(std::string path) {
    /*  read the file data  */
    reader = vtkXMLUnstructuredGridReader::New();
    ugrid  = vtkUnstructuredGrid::New();

    /*  read the unstructured data information from local */
    reader->SetFileName(file.c_str());
    reader->Update();
    ugrid = reader->GetOutput();
    port  = reader->GetOutputPort();

    /*  get the number of fields  */
    pointData     = ugrid->GetPointData();
    numPointField = pointData->GetNumberOfArrays();
    cellData      = ugrid->GetCellData();
    numCellField  = cellData->GetNumberOfArrays();
}
