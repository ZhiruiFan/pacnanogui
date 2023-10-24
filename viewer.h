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
#include <vtkCellData.h>
#include <vtkCellType.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkWarpVector.h>
#include <vtkXMLUnstructuredGridReader.h>

#include <QList>
#include <QObject>
#include <QWidget>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>

/*  class Viewer: */

class Viewer : public QWidget {
    Q_OBJECT

private:
    /*  Viewport variables  */
    QVTKOpenGLNativeWidget* win;
    vtkGenericOpenGLRenderWindow* renWin;
    vtkRenderer* render;
    vtkNamedColors* colors;

    /*  data mapper  */
    vtkDataSetMapper* dtMap;

    /*  actors to show the field  */
    vtkActor* actor;
    vtkOrientationMarkerWidget* axis;
    vtkAxesActor* actaxis;
    std::stringstream time;
    vtkTextActor* status;
    vtkScalarBarActor* scalarBar;

    /*  lookup table  */
    vtkLookupTable* lut;

    /*  data information  */
    vtkDoubleArray* dtOld;
    vtkDoubleArray* dtCur;
    QMap<int, char> compName = {{0, 'X'}, {1, 'Y'}, {2, 'Z'}};

private:
    /*  nested class: Model
     *  Description: the class to read and configure the field variables of each
     *  data files, which includes the point data and cell data  */
    class Model {
    public:
        int index;                             // the index of the field
        int numPointField;                     // number of point data
        int numCellField;                      // number of cell data
        std::string file;                      // the file path
        vtkUnstructuredGrid* ugrid;            // the unstructured grid object
        vtkPointData* pointData;               // the point data object
        vtkCellData* cellData;                 // the cell data object
        vtkAlgorithmOutput* port;              // the data algorithm port
        vtkXMLUnstructuredGridReader* reader;  // reader of the data

    public:
        /*  constructor: create a field objective
         *  @param  file: the path to read the vtu file  */
        Model(std::string path);
    };
    QList<Model*> modelList;

public:
    /*  constructor: create the render window to show the FEM model
     *  @param  window: the openGL widget   */
    Viewer(QVTKOpenGLNativeWidget* window);

    /*  destructor: destroy the render object  */
    ~Viewer();

    /*  showModel: display the geometry of the model
     *  @param  index: the index of the model that will be shown */
    void showModel(int& index);

    /*  showMesh: display the mesh of the FEM model
     *  @param  index: the index of the model that will be shown */
    void showMesh(int& index);

    /*  showField: display the field information
     *  @ param  type: the type of the field variable  */
    void showField(int& index, int& comp);

    /*  extract the point and cell data  */
    void extractFieldData(vtkUnstructuredGrid*& grid);

private:
    /*  configure the small widget in the render window  */
    void configCamera();  // configure the camera

    /*  configScalarBar: configure the scalar bar style  */
    void configScalarBar();

    /*  configure the status bar, which includes the system time, model
     *  information
     *  @param  file: the model that will be displayed
     *  @param  info: the model information that will be shown  */
    void configStatusBar(std::string& file, std::string& info);
};

#endif  // VIEWER_H
