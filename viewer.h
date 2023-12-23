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
#include <vtkDoubleArray.h>
#include <vtkExtractSelection.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImplicitBoolean.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkOrientationMarkerWidget.h>
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
#include <sstream>

#include "camera.h"
#include "field.h"
#include "pick.h"
#include "post.h"

/*  ############################################################################
 *  class Viewer: the class to define the visualization interface, which
 *      includes displaying the geometry, mesh, and field variables, and also
 *      selecting the nodes and elements.  */
class Viewer : public QWidget {
    Q_OBJECT

private:
    Field* field;                                  // filed to be shown
    int recorder[3];                               // filed varaible recorder
    bool isModelCreated;                           // field assignment flag
    bool isModelLoaded;                            // has model loaded
    bool isModelMode;                              // model mode is shown
    bool isFieldMode;                              // field mode is shown
    bool isPickHideModel;                          // will hide picked cells

    Camera* camera;                                // camera configuration
    Post* post;                                    // postprocessing config

    vtkAlgorithmOutput* portCur;                   // current port
    vtkUnstructuredGrid* ugridCur;                 // current ugrid

    QVTKOpenGLNativeWidget* win;                   // main window
    vtkGenericOpenGLRenderWindow* renWin;          // render window
    vtkRenderer* render;                           // render object
    vtkActor* actor;                               //  actor of the viewerport
    vtkDataSetMapper* dtMap;                       // data set mapper
    vtkNamedColors* colors;                        // color object

    vtkLookupTable* lut;                           // lookup table
    vtkCamera* originCamera;                       // original camera
    vtkAxesActor* actaxis;                         // axis actor
    vtkOrientationMarkerWidget* axis;              // axis object
    vtkScalarBarActor* scalarBar;                  // scalar bar
    bool isScalarBarPlayed;                        // the scalarbar is acted
    vtkTextActor* status;                          // status bar
    std::stringstream time;                        // current time

    vtkAlgorithmOutput* pickSource;                // source for picking
    vtkThreshold* pickThreshold;                   // threshold for picker
    Pick* pick;                                    // pick object
    vtkRenderWindowInteractor* interact;           // interactor
    vtkInteractorStyleTrackballCamera* initStyle;  // initial style

    vtkSelectionNode* nodeSelector;                // selection node
    vtkSelection* cellSelector;                    // cell selection
    vtkExtractSelection* extractor;                // extract operator
    vtkIdTypeArray* cellIdsCur;                    // currently selected cells
    vtkIdTypeArray* cellIdsAll;                    // the all selected cells
    vtkCellLocator* locator;                       // cell locator
    vtkPoints* points;                             // points in cell
    double cellCenter[3];                          // cell center location
    double point[3];                               // point location

    /*  data information  */
    QMap<int, char> compName = {{0, 'X'}, {1, 'Y'}, {2, 'Z'}};

public:
    /*  ########################################################################
     *  constructor: create the render window to show the FEM model
     *  @param  window: the openGL widget   */
    Viewer(QVTKOpenGLNativeWidget* window);

    /*  destructor: destroy the render object  */
    ~Viewer();

    /*  configCamera: show the dialog to configuration camera paramerters*/
    void configCamera() { camera->show(); }

    /*  configPost: show the dialog to configure the postprocess  */
    void configPost() { post->show(); };

    /*  showScalarBar: configure the style and show the scalar bar */
    void showScalarBar();

    /*  configure the small widget in the render window  */
    void showCameraAxonometric();  // show the axonometric view
    void showCameraXY();           // set the camera to the XY plane
    void showCameraYZ();           // set the camera to the YZ plane
    void showCameraXZ();           // set the camera to the XZ plane

public:
    /*  ########################################################################
     *  setInputData: set the input filed data that will be shown in the render
     *      window.
     *  @param  input: the filed varaible  */
    void setInputData(Field*& input);

    /*  showCompleteModel: reset the unstructured grid data to the original and
     *      show the model as current configuration  */
    void showCompleteModel();

public:
    /*  showModel: display the geometry of the model
     *  @param  field: the field variable to be shown  */
    void showModel();

    /*  showMesh: display the mesh of the FEM model
     *  @param  field: the field variable to be shown  */
    void showMesh();

public:
    /*  ########################################################################
     *  pickCells: pick up the cells in the viewerport using the mouse box
     *  selection or mouse click method.
     *  @param  mode: node or element selection mode. If true, the node
     *                selection mode is activated. Otherwise, the element
     *                selection mode is activated.  */
    void activePickMode(const bool& mode);

    /*  hideCells: hide the cells selected by the picker  */
    void handleCellPick(const bool& pickMode);

    /*  extractCells: show the cells selected by the picker  */
    void extractCells();

    /*  isPickerActivated: get the status of the picker that is actived or not?
     *  @return  the picker status  */
    bool isPickerActivated() { return pick->isPickerActivated(); }

    /*  turnOffPickMode: turn off the picker mode  */
    void turnOffPickMode() {
        pick->turnOff();
        interact->SetInteractorStyle(initStyle);
    };

public:
    /*  ########################################################################
     *  initPointField: initialize the specified point field from the original
     *  vtu files
     *  @param  idx: the index of point data that will be shown
     *  @param  comp: the component of the point data that will be extracted  */
    void initPointField(const int& idx, const int& comp);

    /*  showPointField: display the information with respect to the nodes,
     *  it includes the nodal displacement, reaction force and so on
     *  @param  idx: the index of the component in the data set
     *  @param  comp: the component index  */
    void showPointField(const int& idx, const int& comp);

    /*  showCellField: display the information with respect to the elements,
     *  it includes the stress components, design variables in topology
     *  optimization and so on
     *  @param  idx: the index of the component in the data set  */
    void showCellField(const int& idx);

private:
    /*  ########################################################################
     *  configure the status bar, which includes the system time, model
     *  information
     *  @param  file: the model that will be displayed
     *  @param  info: the model information that will be shown  */
    void configStatusBar(QString& file, QString& info);

    /*  update: update the displayed object using the current port  */
    void update();
};
#endif  // VIEWER_H
