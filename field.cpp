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

#include "field.h"

#include <QDebug>

/*  ############################################################################
 *  CLASS Field: the class to define the filed that will have a interaction with
 *      the Viewer object. It includes the nodes, elements, vector field (
 *      displacement field, reaction force and so on), scalar field (material,
 *      Mises stress, design variables in Topology optimization and so on)
 *  constructor : create the Field object.
 *  @param name : the name of the field */
Field::Field(QString& _name) {
    //  assign the name of the Field
    name = _name;

    //  define the reader
    reader = vtkXMLUnstructuredGridReader::New();
    reader->SetFileName(name.toStdString().c_str());
    reader->Update();

    //  read the informaiton of the vtu file
    ugrid = reader->GetOutput();
    port  = reader->GetOutputPort();

    //  get the filed data
    pointData = ugrid->GetPointData();
    cellData  = ugrid->GetCellData();

    //  warp vector
    warp      = vtkWarpVector::New();
    ifMeshed  = true;
    warpScale = 200.0;
    warp->SetInputData(ugrid);
}

/*  ============================================================================
 *  destructor: destroy the vtk related object, such as reader, ugrid, point
 *  data, cell data, port, set, warp and so on   */
Field::~Field() {
    port->Delete();
    cellData->Delete();
    pointData->Delete();
    warp->Delete();
    ugrid->Delete();
    reader->Delete();
}

/*  ############################################################################
 *  updateWarper: update the configuration of the warper, which is used to
 *  show the deformed configuration of the FEM model
 *  @return  the status in warpper updating  */
bool Field::updateWarper() {
    /*  Ge the anchor of the warpping  */
    int idx = 0;
    for (idx = 0; idx < pointData->GetNumberOfArrays(); ++idx) {
        if (std::strcmp(pointData->GetArrayName(idx), "U") == 0) break;
    }
    //  check the status
    if (idx == pointData->GetNumberOfArrays()) return false;

    /*  set the anchor of the warpper  */
    warp->SetInputArrayToProcess(0, 0, 0,
                                 vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                 pointData->GetArray(idx)->GetName());
    warp->SetScaleFactor(warpScale);
    warp->Update();
    return true;
}
