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
    /*  assign the name of the filed  */
    name = _name;

    /*  setup the ugrd reader  */
    reader = vtkXMLUnstructuredGridReader::New();
    reader->SetFileName(name.toStdString().c_str());
    reader->Update();

    /*  read the field data  */
    ugrid     = reader->GetOutput();
    portAll   = reader->GetOutputPort();
    pointData = ugrid->GetPointData();
    cellData  = ugrid->GetCellData();

    /*  create the warpper object  */
    warp = vtkWarpVector::New();
    warp->SetInputData(ugrid);

    /*  create the threshold  */
    threshold = vtkThreshold::New();
}

/*  ============================================================================
 *  destructor: destroy the vtk related object, such as reader, ugrid, point
 *  data, cell data, port, set, warp and so on   */
Field::~Field() {
    /*  delete the variables  */
    portAll->Delete();
    cellData->Delete();
    pointData->Delete();
    warp->Delete();
    ugrid->Delete();
    reader->Delete();

    /*  assign the variable to null  */
    portAll   = nullptr;
    portCur   = nullptr;
    cellData  = nullptr;
    pointData = nullptr;
    ugrid     = nullptr;
    reader    = nullptr;
}

/*  ============================================================================
 *  checkAnchor: check the anchor filed variable is included or not?
 *  @return  the checked status, ture for sucessed, otherwise failed  */
bool Field::checkAnchor() {
    /*  define the temporary varaible  */
    int idx = 0;

    /*  extract the anchor of the warpping  */
    numPointField = pointData->GetNumberOfArrays();
    for (idx = 0; idx < numPointField; ++idx) {
        if (std::strcmp(pointData->GetArrayName(idx), "U") == 0) {
            idxU = idx;
            break;
        }
    }
    /*  check the warping anchor  */
    if (idxU >= numPointField) return false;

    /*  extract the anchor of the threshold  */
    numCellField = cellData->GetNumberOfArrays();
    for (idx = 0; idx < numCellField; ++idx) {
        if (std::strcmp(cellData->GetArrayName(idx), "Var-0") == 0) {
            idxDen = idx;
            break;
        }
    }
    /*  check the threshold status  */
    if (idxDen >= numCellField) return false;

    /*  return successed  */
    return true;
}

/*  ============================================================================
 *  updateAnchor: update the anchor field
 *  @param  scale: the warping scale
 *  @param  lower: the lower limit of the threshold
 *  @param  upper: the uppre limit of the threshold  */
void Field::updateAnchor(const double& scale, const double& lower,
                         const double& upper) {
    /*  update the warper  */
    warp->SetInputArrayToProcess(0, 0, 0,
                                 vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                 pointData->GetArray(idxU)->GetName());
    warp->SetInputConnection(portCur);
    warp->SetScaleFactor(scale);
    warp->Update();

    /*  update the threshold  */
    threshold->SetInputConnection(warp->GetOutputPort());
    threshold->SetInputData(warp->GetOutput());
    threshold->SetInputArrayToProcess(0, 0, 0,
                                      vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                      cellData->GetArray(idxDen)->GetName());
    threshold->SetLowerThreshold(lower);
    threshold->SetUpperThreshold(upper);
    threshold->Update();
}
