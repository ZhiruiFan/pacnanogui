/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : viewer.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : October 22th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef VIEWER_H
#define VIEWER_H

#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDoubleArray.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractSelection.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImplicitBoolean.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPlanes.h>
#include <vtkProp.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include <QList>
#include <QObject>
#include <QWidget>
#include <ctime>
#include <sstream>

#include "camera.h"
#include "field.h"
#include "pick.h"

/*  ############################################################################
 *  class Viewer: the class to define the visualization interface, which
 *      includes displaying the geometry, mesh, and field variables, and also
 *      selecting the nodes and elements.  */
class Viewer : public QWidget {
    Q_OBJECT

private:
    Field* field;                                  // filed to be shown
    int recorder[3];                               // filed varaible recorder
    bool isAssigedField;                           // field assignment flag

    Camera* camera;                                // camera configuration

    QVTKOpenGLNativeWidget* win;                   // main window
    vtkGenericOpenGLRenderWindow* renWin;          // render window
    vtkRenderer* render;                           // render object
    vtkNamedColors* colors;                        // color object
    vtkDataSetMapper* dtMap;                       // data set mapper
    vtkLookupTable* lut;                           // lookup table
    vtkCamera* originCamera;                       // original camera

    vtkActor* actor;                               //  actor of the viewerport
    vtkAxesActor* actaxis;                         // axis actor
    vtkOrientationMarkerWidget* axis;              // axis object
    vtkScalarBarActor* scalarBar;                  // scalar bar
    bool isScalarBarPlayed;                        // the scalarbar is acted
    vtkTextActor* status;                          // status bar
    std::stringstream time;                        // current time

    Pick* pick;                                    // pick object
    vtkUnstructuredGrid* ugridCur;                 // current ugrid
    vtkRenderWindowInteractor* interact;           // interactor
    vtkInteractorStyleTrackballCamera* initStyle;  // initial style
    vtkImplicitBoolean* frustum;                   // picker frustum

    vtkExtractGeometry* extractGeo;                // model clip handler
    bool isCliped;                                 // clip flag

    vtkSelectionNode* idsSelector;                 // selection node
    vtkSelection* cellSelector;                    // cell selection
    vtkExtractSelection* extractor;                // extract operator

    /*  data information  */
    QMap<int, char> compName = {{0, 'X'}, {1, 'Y'}, {2, 'Z'}};

public:
    /*  constructor: create the render window to show the FEM model
     *  @param  window: the openGL widget   */
    Viewer(QVTKOpenGLNativeWidget* window);

    /*  destructor: destroy the render object  */
    ~Viewer();

    /*  configCamera: config the parameters of the camera  */
    void configCamera() { camera->show(); }

    /*  setInputData: set the input filed data that will be shown in the render
     *      window.
     *  @param  input: the filed varaible  */
    void setInputData(Field*& input);

    /*  showCompleteModel: reset the unstructured grid data to the original and
     *      show the model as current configuration  */
    void showCompleteModel();

    /*  showModel: display the geometry of the model
     *  @param  field: the field variable to be shown  */
    void showModel();

    /*  showMesh: display the mesh of the FEM model
     *  @param  field: the field variable to be shown  */
    void showMesh();

    /*  showPointField: display the information with respect to the nodes,
     *  it includes the nodal displacement, reaction force and so on
     *  @param  field: the field that to be shown
     *  @param  idx: the index of the component in the data set
     *  @param  comp: the component index  */
    void showPointField(const int& idx, const int& comp);

    /*  showCellField: display the information with respect to the elements,
     *  it includes the stress components, design variables in topology
     *  optimization and so on
     *  @param  field: the field that to be shown
     *  @param  idx: the index of the component in the data set  */
    void showCellField(const int& idx);

    /*  extract the point and cell data  */
    void extractFieldData(vtkUnstructuredGrid*& grid);

    /*  pickCells: pick up the cells in the viewerport using the mouse box
     *  selection method.
     *  @param  filed: the field variable that will be shown
     *  @param  mode: node or element selection mode. If true, the node
     *                selection mode is activated. Otherwise, the element
     *                selection mode is activated.  */
    void pickupCells(bool mode);

    /*  configure the small widget in the render window  */
    void configCameraGeneral();  // configure the camera
    void configCameraXY();       // set the camera to the XY plane
    void configCameraYZ();       // set the camera to the YZ plane
    void configCameraXZ();       // set the camera to the XZ plane
    void configScalarBar();      // scalar bar

    /*  hideSelectedCells: hide the cells selected by the picker  */
    void hideSelectedCells();

    /*  showSelectedCells: show the cells selected by the picker  */
    void showSelectedCells();

private:
    /*  configure the status bar, which includes the system time, model
     *  information
     *  @param  file: the model that will be displayed
     *  @param  info: the model information that will be shown  */
    void configStatusBar(QString& file, QString& info);
};

#endif  // VIEWER_H
