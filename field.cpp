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
    port      = reader->GetOutputPort();
    pointData = ugrid->GetPointData();
    cellData  = ugrid->GetCellData();

    /*  create the warpper object  */
    warp      = vtkWarpVector::New();
    warpScale = 200.0;
    warp->SetInputData(ugrid);

    /*  create the threshold  */
    threshold  = vtkThreshold::New();
    lowerLimit = 0.0;
    upperLimit = 1.0;
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
    if (idxU >= numPointField)
        return false;
    else
        updateWarper(warpScale);

    /*  extract the anchor of the threshold  */
    numCellField = cellData->GetNumberOfArrays();
    for (idx = 0; idx < numCellField; ++idx) {
        if (std::strcmp(cellData->GetArrayName(idx), "Var-0") == 0) {
            idxDen = idx;
            break;
        }
    }
    /*  check the threshold status  */
    if (idxDen >= numCellField)
        return false;
    else
        updateThreshold(lowerLimit, upperLimit);
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
    updateWarper(scale);

    /*  update the threshold  */
    updateThreshold(lower, upper);
}

/*  ############################################################################
 *  updateWarper: update the configuration of the warper, which is used to
 *  show the deformed configuration of the FEM model
 *  @param  scale: the warping scale factor  */
void Field::updateWarper(const double& scale) {
    /*  check the warping scale is changed  */
    /*  update the warping scale  */
    warpScale = scale;

    /*  set the anchor of the warpper  */
    warp->SetInputArrayToProcess(0, 0, 0,
                                 vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                 pointData->GetArray(idxU)->GetName());
    warp->SetScaleFactor(warpScale);
    warp->Update();
}

/*  ============================================================================
 *  updateThreshold: update the threshold according to the optimized element
 *  density, i.e., the cells not in the given limits will be hiden
 *  @param  lower: the lower limit of the threshold
 *  @param  upper: the higher limit of the threshold  */
void Field::updateThreshold(const double& lower, const double& upper) {
    /*  check the limits is updated or not  */
    /*  update the lower and upper limits  */
    lowerLimit = lower;
    upperLimit = upper;

    /*  update the anchor the threshold  */
    threshold->SetInputConnection(warp->GetOutputPort());
    threshold->SetInputData(warp->GetOutput());
    threshold->SetInputArrayToProcess(0, 0, 0,
                                      vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                      cellData->GetArray(idxDen)->GetName());
    threshold->SetLowerThreshold(lowerLimit);
    // threshold->SetUpperThreshold(upperLimit);
    threshold->Update();
}
