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

    /*  cell picking  */
    isPicked  = false;
    pickArray = vtkDoubleArray::New();
    pickArray->SetNumberOfTuples(cellData->GetArray(0)->GetNumberOfTuples());
    pickArray->SetNumberOfComponents(1);
    pickArray->Fill(1.0);
    pickArray->SetName("PickCells");
    cellData->SetScalars(pickArray);

    /*  create the warpper object  */
    warp = vtkWarpVector::New();
    warp->SetInputData(ugrid);

    /*  create the threshold  */
    denFilter  = vtkThreshold::New();
    pickFilter = vtkThreshold::New();

    /*  coutour filter  */
    contourFilter = vtkContourFilter::New();

    /*  initialize the anchor  */
    limitType  = 0;
    warpScale  = 0.0;
    lowerLimit = 0.0;
    upperLimit = 1.0;
    checkAnchor();
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

/*  ############################################################################
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
 *  updateAnchor: update the anchor field  */
void Field::updateAnchor() {
    /*  update the warper  */
    warp->SetInputArrayToProcess(0, 0, 0,
                                 vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                 pointData->GetArray(idxU)->GetName());
    warp->SetInputConnection(reader->GetOutputPort());
    warp->SetScaleFactor(warpScale);
    warp->Update();

    /*  handle for the limit type, i.e., determine the lower and upper limit  */
    switch (limitType) {
        case 0: {
            lowerLimit = cellData->GetArray(idxDen)->GetRange()[0];
            upperLimit = cellData->GetArray(idxDen)->GetRange()[1];
            break;
        }
        case 2: {
            upperLimit = cellData->GetArray(idxDen)->GetRange()[1];
            break;
        }
        case 3: {
            lowerLimit = cellData->GetArray(idxDen)->GetRange()[0];
            break;
        }
    }

    /*  update the threshold  */
    denFilter->SetInputConnection(warp->GetOutputPort());
    denFilter->SetInputData(warp->GetOutput());
    denFilter->SetInputArrayToProcess(0, 0, 0,
                                      vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                      cellData->GetArray(idxDen)->GetName());
    denFilter->SetLowerThreshold(lowerLimit);
    denFilter->SetUpperThreshold(upperLimit);
    denFilter->Update();

    /*  update the data and port  */
    ugridCur = denFilter->GetOutput();
    portCur  = denFilter->GetOutputPort();
}

/*  ############################################################################
 *  setWarpScale: set the coefficient of the warping scale
 *  @param  scale: the warping scale  */
void Field::setWarpScale(const double& scale) { warpScale = scale; }

/*  ============================================================================
 *  setLimitType: set the type of the threshold limination
 *  @param  type: the type of the limit
 *  @param  lower: the lower limit value
 *  @param  upper: the upper limit value  */
void Field::setLimits(const int& type, const double& lower,
                      const double& upper) {
    limitType  = type;
    lowerLimit = lower;
    upperLimit = upper;
}

/*  ============================================================================
 *  getPointDataArray: get the point data from the field
 *  @param  name: the name of the point data
 *  @return  the point data with specified name  */
vtkDataArray* Field::getPointDataArray(const char* name) {
    return pointData->GetArray(name);
}

/*  ============================================================================
 *  getPointDataArray: get the array amoung the all point data
 *  @param  idx: the index amoung the all point data
 *  @return  the point data with specified index  */
vtkDataArray* Field::getPointDataArray(const int& idx) {
    return pointData->GetArray(idx);
}

/*  ============================================================================
 *  getPointDataArrayName: get the name of the point data according to the
 *  specified index
 *  @param  idx: the index of the point data
 *  @return  the name of the point data array  */
const char* Field::getPointDataArrayName(const int& idx) {
    return pointData->GetArrayName(idx);
}

/*  ########################################################################
 *  addPointData: add a new point data to the field
 *  @param  pointDataArray: the array will be added to the field  */
void Field::addPointData(vtkDoubleArray* data) {
    pointData = reader->GetOutput()->GetPointData();
    pointData->SetScalars(data);
}

/*  ############################################################################
 *  performCellPick: do the cell picking operation
 *  @param  isModelMode: picking for model or field?
 *  @param  isHided: hide cells or extract cells
 *  @param  cellIdsCur: the selected cells that will be used for picking */
void Field::performCellPick(const bool isModelMode, const bool isHideMode,
                            vtkIdTypeArray* cellIdsCur) {
    /*  update the picking flag  */
    isPicked = true;

    /*  handling for the model or field mode  */
    if (isModelMode) {
        //  using the model mode
        ugridCur  = ugrid;
        portCur   = portAll;
        pickArray = cellData->GetArray("PickCells");

    } else {
        /*  get the array of the picked array */
        ugridCur  = denFilter->GetOutput();
        portCur   = denFilter->GetOutputPort();
        pickArray = denFilter->GetOutput()
                        ->GetCellData()  //
                        ->GetArray("PickCells");
    }

    /*  handling for selection mode  */
    if (isHideMode) {
        pickValue = 0.0;
    } else {
        pickValue = 1.0;
        pickArray->Fill(0.0);
    }

    /*  assign the value with respect to the picked cells  */
    for (vtkIdType i = 0; i < cellIdsCur->GetNumberOfValues(); ++i) {
        pickArray->SetTuple1(cellIdsCur->GetValue(i), pickValue);
    }

    /*  perform the extracting or hiding operation  */
    pickFilter->SetLowerThreshold(0.5);
    pickFilter->SetInputConnection(portCur);
    pickFilter->SetInputData(ugridCur);
    pickFilter->SetInputArrayToProcess(
        0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "PickCells");
    pickFilter->Update();

    /*  update the data and port  */
    ugridCur = pickFilter->GetOutput();
    portCur  = pickFilter->GetOutputPort();
}

/*  ============================================================================
 *  resetCellPick: reset the cell picking  */
void Field::resetCellPick() {
    /*  reset the pick array  */
    pickArray = cellData->GetArray("PickCells");
    pickArray->Fill(1.0);

    /*  update the port and data  */
    ugridCur  = reader->GetOutput();
    portCur   = reader->GetOutputPort();
    pointData = ugrid->GetPointData();
    cellData  = ugrid->GetCellData();

    /*  update the anchor  */
    updateAnchor();

    /*  update the flag  */
    isPicked = false;
}

/*  ########################################################################
 *  getPathName: get the full path name of the field varaible  */
QString& Field::getPathName() { return name; }

/*  ############################################################################
 *  getInputPort: get the initial port, i.e., the input port of the field
 *  variables  */
vtkAlgorithmOutput* Field::getInputPort() { return portAll; }

/*  getInputData: get the initial unstructured grid of the field
 *  @return  the ugrid  */
vtkUnstructuredGrid* Field::getInputData() { return reader->GetOutput(); }

/*  ============================================================================
 *  getWarpOutputPort: get the output port of the warper
 *  @return  the data port after warping operation  */
vtkAlgorithmOutput* Field::getWarpOutputPort() { return warp->GetOutputPort(); }

/*  getWarpOutput: get the output data of the warper
 *  @return  the data port after warping operation  */
vtkPointSet* Field::getWarpOutput() { return warp->GetOutput(); }

/*  ============================================================================
 *  getThresholdOutputPort: get the output port of the threshold
 *  @return  the data port after threshold operation  */
vtkAlgorithmOutput* Field::getThresholdOutputPort() {
    return denFilter->GetOutputPort();
}

/*  getThresholdOutput: get the output ugrid of the threshold
 *  @return  the data after threshold operation  */
vtkUnstructuredGrid* Field::getThresholdOutput() {
    return denFilter->GetOutput();
}

/*  ============================================================================
 *  getPickOutputPort: get the output port after picking operation
 *  @return  the port of the pick filter  */
vtkAlgorithmOutput* Field::getPickOutputPort() {
    return pickFilter->GetOutputPort();
}

/*  getPickOutput: get the output data after picking operation
 *  @return  the data after the pick filter  */
vtkUnstructuredGrid* Field::getPickOutput() { return pickFilter->GetOutput(); }

/*  ============================================================================
 *  getCellDataArray: get the array amoung the all cell data
 *  @param  idx: the index of the cell data array  */
vtkDataArray* Field::getCellDataArray(const int& idx) {
    return cellData->GetArray(idx);
}

/*  ============================================================================
 *  getContourOutput: get the output data of the contour
 *  @return  the ouput data after contour filter  */
vtkPolyData* Field::getContourOutput() { return contourFilter->GetOutput(); }

/*  getContourOutputPort: get the port with respect to contour filter
 *  @return  the port after contour filter  */
vtkAlgorithmOutput* Field::getContourOutputPort() {
    return contourFilter->GetOutputPort();
}
