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
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkThreshold.h>
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
    QString name;                          // the name of the filed
    int numPointField;                     // number of nodal field variables
    int numCellField;                      // number of element filed variables

    bool ifMeshed;                         // whether show the mesh

    vtkXMLUnstructuredGridReader* reader;  // reader of the vtu file
    vtkUnstructuredGrid* ugrid;            // grid of the FEM model
    vtkAlgorithmOutput* portAll;           // complete port of vtu file

    vtkUnstructuredGrid* ugridCur;         // current operated data
    vtkAlgorithmOutput* portCur;           // current port

    vtkPointData* pointData;               // point data of the vtu file
    int idxU;                              // index of the displacement field
    vtkWarpVector* warp;                   // warper of the FEM mdoel
    double warpScale;                      // warp scale coefficient

    vtkThreshold* denFilter;               // threshold of cell data
    vtkCellData* cellData;                 // cell data of vtu file
    int idxDen;                            // index of element density
    int limitType;                         // the limit type
    double upperLimit;                     // upper limit of displayed field
    double lowerLimit;                     // lower limit of displayed field
    double dataRange[2];                   // range of the current field

    vtkThreshold* pickFilter;              // filter for picking
    vtkDataArray* pickArray;               // array for picking
    double pickValue;                      // value for picked sequence

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

    /*  setFieldRange: set the range of the field data in current viewerport
     *  @param  range: the range of the field data  */
    void setFieldRange(double*& range);

    /*  getPointDataArrayName: get the name of the point data according to the
     *  specified index
     *  @param  idx: the index of the point data
     *  @return  the name of the point data array  */
    const char* getPointDataArrayName(const int& idx);

public:
    /*  ########################################################################
     *  setPickInputConnection: set the input port for picking cells
     *  @cellIdsCur: the selected cells that will be used for  */
    void setPickInputConnection(const bool isModelMode, const bool isHided,
                                vtkIdTypeArray* cellIdsCur);

    /*  getPickOutputPort: get the output port after picking operation
     *  @return  the port of the pick filter  */
    vtkAlgorithmOutput* getPickOutputPort();

public:
    /*  ########################################################################
     *  getPathName: get the full path name of the field varaible  */
    QString& getPathName() { return name; }

    /*  getInputPort: get the initial port, i.e., the input port of the field
     *  variables  */
    vtkAlgorithmOutput* getInputPort() { return portAll; }

    /*  getInputData: get the initial unstructured grid of the field
     *  @return  the ugrid  */
    vtkUnstructuredGrid* getInputData() { return ugrid; }

    /*  getWarpOutputPort: get the output port of the warper  */
    vtkAlgorithmOutput* getWarpOutputPort() { return warp->GetOutputPort(); }

    /*  getThresholdOutputPort: get the output port of the threshold  */
    vtkAlgorithmOutput* getThresholdOutputPort() {
        return denFilter->GetOutputPort();
    }

    /*  getThresholdOutput: get the output ugrid of the threshold  */
    vtkUnstructuredGrid* getThresholdOutput() { return denFilter->GetOutput(); }

    /*  getCellDataArray: get the array amoung the all cell data
     *  @param  idx: the index of the cell data array  */
    vtkDataArray* getCellDataArray(const int& idx) {
        return cellData->GetArray(idx);
    }

private:
    /*  createNodalSet: create the node set using the given node sequence or by
     *  selecting from the viewerport  */
    void createNodalSet(double*& seq);
};
#endif  // FIELD_H
