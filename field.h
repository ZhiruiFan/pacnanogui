/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : Field.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : November 22th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */

#ifndef FIELD_H
#define FIELD_H

/*  INCLUDES  */
#include <vtkAlgorithmOutput.h>
#include <vtkAppendFilter.h>
#include <vtkCellData.h>
#include <vtkCleanUnstructuredGrid.h>
#include <vtkContourFilter.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkThreshold.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkWarpVector.h>
#include <vtkXMLUnstructuredGridReader.h>

#include <QString>

/*  ############################################################################
 *  CLASS Field: the class to define the filed that will have a interaction with
 *      the Viewer object. It includes the nodes, elements, vector field (
 *      displacement field, reaction force and so on), scalar field (material,
 *      Mises stress, design variables in Topology optimization and so on)  */
class Field {
private:
    QString name;                           // the name of the filed
    int numPointField;                      // number of nodal field variables
    int numCellField;                       // number of element filed variables

    bool ifMeshed;                          // whether show the mesh

    vtkXMLUnstructuredGridReader* reader;   // reader of the vtu file
    vtkUnstructuredGrid* ugrid;             // grid of the FEM model
    vtkAlgorithmOutput* portAll;            // complete port of vtu file

    vtkUnstructuredGrid* ugridCur;          // current operated data
    vtkAlgorithmOutput* portCur;            // current port

    vtkPointData* pointData;                // point data of the vtu file
    int idxU;                               // index of the displacement field
    vtkWarpVector* warp;                    // warper of the FEM mdoel
    double warpScale;                       // warp scale coefficient

    vtkThreshold* denFilter;                // threshold of cell data
    vtkCellData* cellData;                  // cell data of vtu file
    int idxDen;                             // index of element density
    int limitType;                          // the limit type
    double upperLimit;                      // upper limit of displayed field
    double lowerLimit;                      // lower limit of displayed field
    double dataRange[2];                    // range of the current field

    double pickValue;                       // value for picked sequence
    bool isPicked;                          // has cells been picked
    vtkThreshold* pickFilter;               // filter for picking
    vtkDataArray* pickArray;                // array for picking

    vtkContourFilter* contourFilter;        // contour ploting object

    vtkTransform* transform;                // stransform object
    vtkTransformFilter* transformFilter;    // filter for transforming
    vtkAppendFilter* appendFilter;          // append filter
    vtkCleanUnstructuredGrid* cleanFilter;  // clean the duplicated data

    char compName[3] = {'X', 'Y', 'Z'};     // component name

public:
    /*  ########################################################################
     *  constructor: create the Field object.
     *  @param  name: the name of the field     */
    Field(QString& _name);

    /*  destructor: destroy the vtk related object, such as reader, ugrid, point
     *  data, cell data, port, set, warp and so on   */
    ~Field();

public:
    /*  ########################################################################
     *  checkAnchor: check the anchor filed variable is included or not?
     *  @return  the checked status, ture for sucessed, otherwise failed  */
    bool checkAnchor();

    /*  updateAnchor: update the anchor field  */
    void updateAnchor();

public:
    /*  ########################################################################
     *  addPointData: add a new point data to the field
     *  @param  pointDataArray: the array will be added to the field  */
    void addPointData(vtkDoubleArray* data);

    /*  setWarpScale: set the coefficient of the warping scale
     *  @param  scale: the warping scale  */
    void setWarpScale(const double& scale);

    /*  setLimitType: set the type of the threshold limination
     *  @param  type: the type of the limit
     *  @param  lower: the lower limit value
     *  @param  upper: the upper limit value  */
    void setLimits(const int& type, const double& upper, const double& uper);

    /*  getPointDataArray: get the point data from the field
     *  @param  name: the name of the point data
     *  @return  the point data with specified name  */
    vtkDataArray* getPointDataArray(const char* name);

    /*  getPointDataArray: get the array amoung the all point data
     *  @param  idx: the index amoung the all point data
     *  @return  the point data with specified index  */
    vtkDataArray* getPointDataArray(const int& idx);

    /*  getPointDataArrayName: get the name of the point data according to the
     *  specified index
     *  @param  idx: the index of the point data
     *  @return  the name of the point data array  */
    const char* getPointDataArrayName(const int& idx);

public:
    /*  mirror: mirror the unstructured grid according to the specifed
     *  parameters
     *  @param  isUseAll: whether the all ugrid is used or not?
     *  @param  plane: determine the XY, YZ, XZ plane is used or not?
     *  @param  offset: offset of the original points
     *  @param  rotation: rotation angles along each axis  */
    void mirror(const bool isUseAll, const bool* planes, const double* offset,
                const double* rotation);

    /*  getMirrorOutput: get the ugrid after mirror operation
     *  @return  the unstrictured grid after mirror operation  */
    vtkUnstructuredGrid* getMirrorOutput();

    /*  getMirrorOutput: get the data port after mirror operation
     *  @return  the data port after mirror operation  */
    vtkAlgorithmOutput* getMirrorOutputPort();

public:
    /*  ########################################################################
     *  performCellPick: do the cell picking operation
     *  @param  isModelMode: picking for model or field?
     *  @param  isHided: hide cells or extract cells
     *  @param  cellIdsCur: the selected cells that will be used for picking */
    void performCellPick(const bool isModelMode, const bool isHided,
                         vtkIdTypeArray* cellIdsCur);

    /*  getPickOutputPort: get the output port after picking operation
     *  @return  the port of the pick filter  */
    vtkAlgorithmOutput* getPickOutputPort();

    /*  getPickOutput: get the output data after picking operation
     *  @return  the data after the pick filter  */
    vtkUnstructuredGrid* getPickOutput();

    /*  resetCellPick: reset the cell picking  */
    void resetCellPick();

    /*  getCurrentPointDataRange: get the currently displayed scalar range
     *  @param  idx: the index of the point data
     *  @param  comp: the component in the point data  */
    double* getCurrentPointDataRange(const int idx, const int comp);

public:
    /*  ########################################################################
     *  getPathName: get the full path name of the field varaible  */
    QString& getPathName();

    /*  getInputPort: get the initial port, i.e., the input port of the field
     *  variables  */
    vtkAlgorithmOutput* getInputPort();

    /*  getInputData: get the initial unstructured grid of the field
     *  @return  the ugrid  */
    vtkUnstructuredGrid* getInputData();

    /*  getWarpOutputPort: get the output port of the warper
     *  @return  the data port after warping operation  */
    vtkAlgorithmOutput* getWarpOutputPort();

    /*  getWarpOutput: get the output data of the warper
     *  @return  the data port after warping operation  */
    vtkPointSet* getWarpOutput();

    /*  getThresholdOutputPort: get the output port of the threshold
     *  @return  the data port after threshold operation  */
    vtkAlgorithmOutput* getThresholdOutputPort();

    /*  getThresholdOutput: get the output ugrid of the threshold
     *  @return  the data after threshold operation  */
    vtkUnstructuredGrid* getThresholdOutput();

    /*  getCellDataArray: get the array amoung the all cell data
     *  @param  idx: the index of the cell data array  */
    vtkDataArray* getCellDataArray(const int& idx);

public:
    /*  ########################################################################
     *  initContour: initialize the environment for the contour plot*/
    void initContour();

    /*  getContourOutput: get the output data of the contour
     *  @return  the ouput data after contour filter  */
    vtkPolyData* getContourOutput();

    /*  getContourOutputPort: get the port with respect to contour filter
     *  @return  the port after contour filter  */
    vtkAlgorithmOutput* getContourOutputPort();

private:
    /*  createNodalSet: create the node set using the given node sequence or by
     *  selecting from the viewerport  */
    void createNodalSet(double*& seq);
};
#endif  // FIELD_H
