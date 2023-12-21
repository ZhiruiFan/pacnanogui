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
    vtkAlgorithmOutput* portCur;           // current port

    vtkPointData* pointData;               // point data of the vtu file
    int idxU;                              // index of the displacement field
    vtkWarpVector* warp;                   // warper of the FEM mdoel

    vtkThreshold* threshold;               // threshold of cell data
    vtkCellData* cellData;                 // cell data of vtu file
    int idxDen;                            // index of element density

public:
    /*  ########################################################################
     *  constructor: create the Field object.
     *  @param  name: the name of the field     */
    Field(QString& _name);

    /*  destructor: destroy the vtk related object, such as reader, ugrid, point
     *  data, cell data, port, set, warp and so on   */
    ~Field();

    /*  addPointData: add a new point data to the field
     *  @param  pointDataArray: the array will be added to the field  */
    void addPointData(vtkDoubleArray* data) { pointData->SetScalars(data); }

    /*  getPointData: get the point data from the field  */
    vtkDataArray* getPointData(const char* name) {
        return pointData->GetArray(name);
    }

    /*  checkAnchor: check the anchor filed variable is included or not?
     *  @return  the checked status, ture for sucessed, otherwise failed  */
    bool checkAnchor();

    /*  setInputConnection: set the input port to show the field variables  */
    void setInputConnection(vtkAlgorithmOutput*& port) { portCur = port; }

    /*  updateAnchor: update the anchor field
     *  @param  scale: the warping scale
     *  @param  lower: the lower limit of the threshold
     *  @param  upper: the uppre limit of the threshold  */
    void updateAnchor(const double& scale, const double& lower,
                      const double& upper);

public:
    /*  ########################################################################
     *  getPathName: get the full path name of the field varaible  */
    QString& getPathName() { return name; }

    /*  getInputPort: get the initial port, i.e., the input port of the field
     *  variables  */
    vtkAlgorithmOutput* getInputPort() { return portAll; }

    /*  getWarpOutputPort: get the output port of the warper  */
    vtkAlgorithmOutput* getWarpOutputPort() { return warp->GetOutputPort(); }

    /*  getThresholdOutputPort: get the output port of the threshold  */
    vtkAlgorithmOutput* getThresholdOutputPort() {
        return threshold->GetOutputPort();
    }

    /*  getPointDataArray: get the array amoung the all point data
     *  @param  idx: the index amoung the all point data  */
    vtkDataArray* getPointDataArray(const int& idx) {
        return pointData->GetArray(idx);
    }

    /*  getCellDataArray: get the array amoung the all cell data
     *  @param  idx: the index of the cell data array  */
    vtkDataArray* getCellDataArray(const int& idx) {
        return cellData->GetArray(idx);
    }

    /*  getPointDataArrayName: get the name of the point data according to the
     *  specified index
     *  @param  idx: the index of the point data  */
    const char* getPointDataArrayName(const int& idx) {
        return pointData->GetArrayName(idx);
    }

private:
    /*  createNodalSet: create the node set using the given node sequence or by
     *  selecting from the viewerport  */
    void createNodalSet(double*& seq);
};

#endif  // FIELD_H
