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
#include <QStringList>

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
    vtkUnstructuredGrid* ugridAll;          // grid of the FEM model
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

    QStringList fieldNameList;              // name list of of the field
    std::string fieldName;                  // field name
    QStringList compNameList;               // name list of the components
    std::string compName;                   // component name

public:
    /*  constructor: create the Field object.
     *  @param  name: the name of the field     */
    Field(QString& _name);

    /*  destructor: destroy the vtk related object, such as reader, ugrid, point
     *  data, cell data, port, set, warp and so on   */
    ~Field();

    /*  getPathName: get the full path name of the field varaible
     *  @return  the path of the current model  */
    QString& getPathName();

    /*  getInputPort: get the initial port, i.e., the input port of the field
     *  variables
     *  @return  the initial port of the field data  */
    vtkAlgorithmOutput* getInputPort();

    /*  getInputData: get the initial unstructured grid of the field
     *  @return  the initial ugrid  */
    vtkUnstructuredGrid* getInputData();

    /*  initliztePointData: initialize the point data in the field  */
    void initializePointData();

    /*  getNumberOfPointData: get the number of the point datas in the field
     *  @return  the number of the point data  */
    int getNumberOfPointData();

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

    /*  addPointData: add a new point data to the field
     *  @param  pointDataArray: the array will be added to the field  */
    void addPointData(vtkDoubleArray* data);

    /*  getNumberOfCellData: get the number of the cell datas in the field
     *  @the number of cell data  */
    int getNumberOfCellData();

public:
    /*  setWarpScale: set the coefficient of the warping scale
     *  @param scale: the warping scale */
    void setWarpScale(const double& scale);

    /*  setLimitType: set the type of the threshold limination
     *  @param  type: the type of the limit
     *  @param  lower: the lower limit value
     *  @param  upper: the upper limit value  */
    void setLimits(const int& type, const double& upper, const double& uper);

    /*  checkAnchor: check the anchor filed variable is included or not?
     *  @return  the checked status, ture for sucessed, otherwise failed  */
    bool checkAnchor();

    /*  updateAnchor: update the anchor field  */
    void updateAnchor();

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

    /*  getPointDataRange: get the range of the point after density filter
     *  operation
     *  @return  the range of the point data  */
    double* getPointDataRange(char* name);

    /*  getCellDataRange: get the range of the cell data after density filter
     *  operation
     *  @return  the range of the cell data  */
    double* getCellDataRange(char* name);

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
    /*  performCellPick: do the cell picking operation
     *  @param  operateType: the source type of the field data
     *  @param  isModelMode: picking for model or field?
     *  @param  isHided: hide cells or extract cells
     *  @param  cellIdsCur: the selected cells that will be used for picking */
    void performCellPick(const int operateType, const bool isModelMode,
                         const bool isHided, vtkIdTypeArray* cellIdsCur);

    /*  getPickOutputPort: get the output port after picking operation
     *  @return  the port of the pick filter  */
    vtkAlgorithmOutput* getPickOutputPort();

    /*  getPickOutput: get the output data after picking operation
     *  @return  the data after the pick filter  */
    vtkUnstructuredGrid* getPickOutput();

    /*  resetCellPick: reset the cell picking according to the operate type
     *  @param  operateType: the type of the operation  */
    void resetCellPick(const int operateType);

    /*  getCurrentPointDataRange: get the currently displayed scalar range
     *  @param  idx: the index of the point data
     *  @param  comp: the component in the point data  */
    double* getCurrentPointDataRange(const int idx, const int comp);

public:
    /*  getFieldNameList: get the list of the field name
     *  @return  the name list of the fields  */
    QStringList& getFieldNameList() { return fieldNameList; }

    /*  getFieldName: get the field name according to the index
     *  @param  index: the index of the field
     *  @return  the name of the field  */
    char* getFieldName(int idx) {
        fieldName.clear();
        fieldName.assign(fieldNameList[idx].toStdString());
        return fieldName.data();
    }

    /*  getCompName: get the name of components in field
     *  @return  the name list of the components  */
    QStringList& getCompNameList() { return compNameList; }

    /*  getCompName: get the field name according to the index
     *  @param  index: the index of the field
     *  @return  the name of the field  */
    char* getCompName(int idx) {
        compName.clear();
        compName.assign(compNameList[idx].toStdString());
        return compName.data();
    }

public:
    /*  getCellDataArray: get the array amoung the all cell data
     *  @param  idx: the index of the cell data array  */
    vtkDataArray* getCellDataArray(const int& idx);

private:
    /*  createNodalSet: create the node set using the given node sequence or by
     *  selecting from the viewerport  */
    void createNodalSet(double*& seq);

    /*  assignFieldNameList: determine the list of combo box in viewerport  */
    void assignFieldNameList();
};
#endif  // FIELD_H
