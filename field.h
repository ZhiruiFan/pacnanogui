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
#include <vtkPointData.h>
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
public:
    int numPointField;                     // number of nodal field variables
    int numCellField;                      // number of element filed variables
    QString name;                          // the name of the filed
    vtkXMLUnstructuredGridReader* reader;  // reader of the vtu file
    vtkUnstructuredGrid* ugrid;            // grid of the FEM model
    vtkPointData* pointData;               // point data of the vtu file
    vtkCellData* cellData;                 // cell data of vtu file
    vtkAlgorithmOutput* port;              // output port of vtu file
    vtkWarpVector* warp;                   // warper of the FEM mdoel
    bool ifMeshed;                         // whether show the mesh
    double warpScale;                      // warp scale

public:
    /*  constructor: create the Field object.
     *  @param  name: the name of the field     */
    Field(QString& _name);

    /*  destructor: destroy the vtk related object, such as reader, ugrid, point
     *  data, cell data, port, set, warp and so on   */
    ~Field();

    /*  updateWarper: update the configuration of the warper, which is used to
     *  show the deformed configuration of the FEM model
     *  @return  the status in warpper updating status  */
    bool updateWarper();

    /*  createNodalSet: create the node set using the given node sequence or by
     *  selecting from the viewerport  */
    void createNodalSet(double*& seq);  // using the given node sequence
};

#endif  // FIELD_H
