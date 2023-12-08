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
#include <vtkAreaPicker.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkCellType.h>
#include <vtkCommand.h>
#include <vtkDataArray.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDoubleArray.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkGeometryFilter.h>
#include <vtkIdFilter.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPlanes.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include <QList>
#include <QObject>
#include <QWidget>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>

#include "field.h"

/*  ############################################################################
 *  class Viewer: the class to define the visualization interface, which
 *      includes displaying the geometry, mesh, and field variables, and also
 *      selecting the nodes and elements.  */
class Viewer : public QWidget {
    Q_OBJECT

private:
    /*  Viewport variables  */
    QVTKOpenGLNativeWidget* win;           // main window
    vtkGenericOpenGLRenderWindow* renWin;  // render window
    vtkRenderer* render;                   // render object
    vtkNamedColors* colors;                // color object
    vtkDataSetMapper* dtMap;               // data set mapper
    vtkActor* actor;                       //  actor of the viewerport
    vtkOrientationMarkerWidget* axis;      // axis object
    vtkAxesActor* actaxis;                 // axis actor
    std::stringstream time;                // current time
    vtkTextActor* status;                  // status bar
    vtkScalarBarActor* scalarBar;          // scalar bar
    bool isScalarBarPlayed;                // the scalarbar is acted or not
    vtkLookupTable* lut;                   // lookup table
    vtkRenderWindowInteractor* interact;   // interactor
    vtkCellPicker* cellPicker;             // cell picker

    /*  data information  */
    QMap<int, char> compName = {{0, 'X'}, {1, 'Y'}, {2, 'Z'}};

private:
    class Pick : public vtkInteractorStyleRubberBandPick {
    private:
        vtkPolyData* polyData;                     // ploy geometry of mesh
        vtkActor* selectActor;                     // actor for selection
        vtkDataSetMapper* selectMap;               // mappler of data selection
        vtkExtractPolyDataGeometry* polyGeometry;  // geometry
        vtkPlanes* frustum;                        // viewerport frustum

    public:
        /*  New: create the object using the VTK style  */
        static Pick* New();
        vtkTypeMacro(Pick, vtkInteractorStyleRubberBandPick);
        /*  Constructor: create the Pick object  */
        Pick() : vtkInteractorStyleRubberBandPick() {
            selectMap   = vtkDataSetMapper::New();
            selectActor = vtkActor::New();
            selectActor->SetMapper(selectMap);
            polyGeometry = vtkExtractPolyDataGeometry::New();
        };

        /*  OnLeftButtonUp: override the event for the left button up  */
        virtual void OnLeftButtonUp() override;

        /*  setPolyData: assign the poly data to the current object  */
        void setPolyData(vtkPolyData* pd) {
            polyData = pd;
            polyGeometry->SetInputData(polyData);
        };
    };

public:
    /*  constructor: create the render window to show the FEM model
     *  @param  window: the openGL widget   */
    Viewer(QVTKOpenGLNativeWidget* window);

    /*  destructor: destroy the render object  */
    ~Viewer();

    /*  showModel: display the geometry of the model
     *  @param  field: the field variable to be shown  */
    void showModel(Field*& field);

    /*  showMesh: display the mesh of the FEM model
     *  @param  field: the field variable to be shown  */
    void showMesh(Field*& field);

    /*  showPointField: display the information with respect to the nodes,
     *  it includes the nodal displacement, reaction force and so on
     *  @param  field: the field that to be shown
     *  @param  idx: the index of the component in the data set
     *  @param  comp: the component index  */
    void showPointField(Field*& field, const int& idx, const int& comp);

    /*  showCellField: display the information with respect to the elements,
     *  it includes the stress components, design variables in topology
     *  optimization and so on
     *  @param  field: the field that to be shown
     *  @param  idx: the index of the component in the data set  */
    void showCellField(Field*& field, const int& idx);

    /*  extract the point and cell data  */
    void extractFieldData(vtkUnstructuredGrid*& grid);

    /*  pickCells: pick up the cells in the viewerport using the mouse box
     *  selection method.  */
    void pickupCells(Field* field);

private:
    /*  configure the small widget in the render window  */
    void configCamera();     // configure the camera
    void configScalarBar();  // scalar bar

    /*  configure the status bar, which includes the system time, model
     *  information
     *  @param  file: the model that will be displayed
     *  @param  info: the model information that will be shown  */
    void configStatusBar(QString& file, QString& info);
};

#endif  // VIEWER_H
