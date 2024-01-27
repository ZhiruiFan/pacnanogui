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

#include "prenano.h"

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
    ugridAll = reader->GetOutput();
    portAll  = reader->GetOutputPort();

    /*  get the field data  */
    pointData     = ugridAll->GetPointData();
    cellData      = ugridAll->GetCellData();
    numPointField = pointData->GetNumberOfArrays();
    numCellField  = cellData->GetNumberOfArrays();
    //  determine the field name list  */
    assignFieldNameList();
    initializePointData();

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
    warp->SetInputData(ugridAll);

    /*  create the threshold  */
    denFilter     = vtkThreshold::New();
    pickFilter    = vtkThreshold::New();
    contourFilter = vtkContourFilter::New();
    cleanFilter   = vtkCleanUnstructuredGrid::New();

    /*  initialize the anchor  */
    limitType  = 0;
    warpScale  = 0.0;
    lowerLimit = 0.0;
    upperLimit = 1.0;

    /*  initialize the anchor  */
    checkAnchor();
    updateAnchor();
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
    ugridAll->Delete();
    reader->Delete();

    /*  assign the variable to null  */
    portAll   = nullptr;
    portCur   = nullptr;
    cellData  = nullptr;
    pointData = nullptr;
    ugridAll  = nullptr;
    reader    = nullptr;
}

/*  ============================================================================
 *  getPathName: get the full path name of the field varaible
 *  @return  the path of the current model  */
QString& Field::getPathName() { return name; }

/*  getInputPort: get the initial port, i.e., the input port of the field
 *  variables
 *  @return  the initial port of the field data  */
vtkAlgorithmOutput* Field::getInputPort() { return reader->GetOutputPort(); }

/*  getInputData: get the initial unstructured grid of the field
 *  @return  the initial ugrid  */
vtkUnstructuredGrid* Field::getInputData() { return reader->GetOutput(); }

/*  ############################################################################
 *  initliztePointData: initialize the point data in the field  */
void Field::initializePointData() {
    /*  define the temporary variables  */
    vtkDataArray* dtOld;     // the old data
    vtkDoubleArray* dtCur;   // the extracted data
    std::stringstream name;  // name of the field components
    double x, y, z;          // variable to store the value of components

    /*  extract the components in the field  */
    //  loop over point data
    for (vtkIdType i = 0; i < numPointField; ++i) {
        /*  get the current point data  */
        dtOld = pointData->GetArray(i);
        /*  extract the components  */
        //  loop over components
        for (vtkIdType j = 0; j < dtOld->GetNumberOfComponents(); ++j) {
            //  initialize the temporary variable
            dtCur = vtkDoubleArray::New();
            dtCur->SetNumberOfComponents(1);
            //  extract the sub components
            for (vtkIdType k = 0; k < dtOld->GetNumberOfTuples(); ++k) {
                dtCur->InsertNextValue(dtOld->GetTuple(k)[j]);
            }
            //  determine the name of the components
            name.str("");
            name << fieldNameList[i].toStdString() << ":"
                 << compNameList[j].toStdString();
            std::string arrayName;
            arrayName.assign(name.str().data());
            dtCur->SetName(name.str().data());
            pointData->AddArray(dtCur);
            //  release the temporary variable
            dtCur->Delete();
        }
        /*  assign the amplitude of current field  */
        //  initialize the temporary variable
        dtCur = vtkDoubleArray::New();
        dtCur->SetNumberOfComponents(1);
        //  calcualte the amplitude
        for (vtkIdType j = 0; j < dtOld->GetNumberOfTuples(); ++j) {
            x = dtOld->GetTuple(j)[0];
            y = dtOld->GetTuple(j)[1];
            z = dtOld->GetTuple(j)[2];
            dtCur->InsertNextValue(sqrt(x * x + y * y + z * z));
        }
        //  determine the name of the components
        name.clear();
        name.str("");
        name << fieldNameList[i].toStdString() << ":"
             << compNameList[3].toStdString();
        dtCur->SetName(name.str().c_str());
        pointData->AddArray(dtCur);
        //  release the temporary variable
        dtCur->Delete();

        /*  remove the unused point data  */
        if (fieldNameList[i].toStdString() !=
            fieldNameList[idxU].toStdString()) {
            pointData->RemoveArray(fieldNameList[i].toStdString().data());
        }
    }
}

/*  ============================================================================
 *  getNumberOfPointData: get the number of the point datas in the field
 *  @return  the number of the point data  */
int Field::getNumberOfPointData() { return pointData->GetNumberOfArrays(); }

/* getPointDataArray: get the point data from the field
 *  @param  name: the name of the point data
 *  @return  the point data with specified name  */
vtkDataArray* Field::getPointDataArray(const char* name) {
    return pointData->GetArray(name);
}

/*  getPointDataArray: get the array amoung the all point data
 *  @param  idx: the index amoung the all point data
 *  @return  the point data with specified index  */
vtkDataArray* Field::getPointDataArray(const int& idx) {
    return pointData->GetArray(idx);
}

/*  getPointDataArrayName: get the name of the point data according to the
 *  specified index
 *  @param  idx: the index of the point data
 *  @return  the name of the point data array  */
const char* Field::getPointDataArrayName(const int& idx) {
    return pointData->GetArrayName(idx);
}

/*  getNumberOfCellData: get the number of the cell datas in the field
 *  @the number of cell data  */
int Field::getNumberOfCellData() { return cellData->GetNumberOfArrays(); }

/*  addPointData: add a new point data to the field
 *  @param  pointDataArray: the array will be added to the field  */
void Field::addPointData(vtkDoubleArray* data) {
    //  get the data name
    pointData = reader->GetOutput()->GetPointData();
    pointData->SetScalars(data);
    //  update the anchor
    updateAnchor();
}

/*  ############################################################################
 *  setWarpScale: set the coefficient of the warping scale
 *  @param  scale: the warping scale  */
void Field::setWarpScale(const double& scale) { warpScale = scale; }

/*  setLimitType: set the type of the threshold limination
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
 *  checkAnchor: check the anchor filed variable is included or not?
 *  @return  the checked status, ture for sucessed, otherwise failed  */
bool Field::checkAnchor() {
    /*  define the temporary varaible  */
    int idx = 0;

    /*  extract the anchor of the warpping  */
    for (idx = 0; idx < numPointField; ++idx) {
        if (std::strcmp(pointData->GetArrayName(idx), "U") == 0) {
            idxU = idx;
            break;
        }
    }
    /*  check the warping anchor  */
    if (idxU >= numPointField) return false;

    /*  extract the anchor of the threshold  */
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
    warp->SetInputData(ugridAll);
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
    denFilter->RemoveAllInputConnections(0);
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

/*  ============================================================================
 *  getWarpOutputPort: get the output port of the warper
 *  @return  the data port after warping operation  */
vtkAlgorithmOutput* Field::getWarpOutputPort() { return warp->GetOutputPort(); }

/*  getWarpOutput: get the output data of the warper
 *  @return  the data port after warping operation  */
vtkPointSet* Field::getWarpOutput() { return warp->GetOutput(); }

/*  getThresholdOutputPort: get the output port of the threshold
 *  @return  the data port after threshold operation  */
vtkAlgorithmOutput* Field::getThresholdOutputPort() {
    return denFilter->GetOutputPort();
}

/*  getThresholdOutput: get the output ugrid of the threshold
 *  @return  the data after threshold operation  */
vtkUnstructuredGrid* Field::getThresholdOutput() {
    return denFilter->GetOutput();
}

/*  getThresholdRange: get the range of the point after density filter
 *  operation
 *  @return  the range of the point data  */
double* Field::getThresholdRange(char* name) {
    return denFilter->GetOutput()->GetPointData()->GetArray(name)->GetRange();
}

/*  ############################################################################
 *  performCellPick: do the cell picking operation
 *  @param  operateType: the source type of the field data
 *  @param  isModelMode: picking for model or field?
 *  @param  isHided: hide cells or extract cells
 *  @param  cellIdsCur: the selected cells that will be used for picking */
void Field::performCellPick(const int operateType, const bool isModelMode,
                            const bool isHideMode, vtkIdTypeArray* cellIdsCur) {
    /*  update the picking flag  */
    isPicked = true;

    /*  handling for the model or field mode  */
    if (isModelMode) {
        //  using the model mode
        ugridCur  = ugridAll;
        portCur   = portAll;
        pickArray = cellData->GetArray("PickCells");

    } else {
        /*  get the array of the picked array */
        switch (operateType) {
            //  original field
            case PRENANO::USE_ORIGIN_FIELD:
                ugridCur  = denFilter->GetOutput();
                portCur   = denFilter->GetOutputPort();
                pickArray = denFilter->GetOutput()
                                ->GetCellData()  //
                                ->GetArray("PickCells");
                break;
            //  mirrored field
            case PRENANO::USE_MIRROR_FIELD:
                ugridCur  = cleanFilter->GetOutput();
                portCur   = cleanFilter->GetOutputPort();
                pickArray = cleanFilter->GetOutput()
                                ->GetCellData()  //
                                ->GetArray("PickCells");
                break;
        }
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
 *  resetCellPick: reset the cell picking according to the operate type
 *  @param  operateType: the type of the operation  */
void Field::resetCellPick(const int operateType) {
    /*  reset the pick array  */

    /*  update the port and data  */
    ugridCur = reader->GetOutput();
    portCur  = reader->GetOutputPort();

    /*  update the filter  */
    switch (operateType) {
        case PRENANO::USE_ORIGIN_FIELD:
            pickArray = denFilter->GetOutput()
                            ->GetCellData()  //
                            ->GetArray("PickCells");
            break;
        case PRENANO::USE_MIRROR_FIELD:
            pickArray = cleanFilter->GetOutput()
                            ->GetCellData()  //
                            ->GetArray("PickCells");
            break;
    }
    pickArray->Fill(1.0);

    /*  update the flag  */
    isPicked = false;
}

/*  getPickOutputPort: get the output port after picking operation
 *  @return  the port of the pick filter  */
vtkAlgorithmOutput* Field::getPickOutputPort() {
    return pickFilter->GetOutputPort();
}

/*  getPickOutput: get the output data after picking operation
 *  @return  the data after the pick filter  */
vtkUnstructuredGrid* Field::getPickOutput() { return pickFilter->GetOutput(); }

/*  ############################################################################
 *  mirror: mirror the unstructured grid according to the specifed
 *  parameters
 *  @param  isUseAll: whether the all ugrid is used or not?
 *  @param  plane: determine the XY, YZ, XZ plane is used or not?
 *  @param  offset: offset of the original points
 *  @param  rotation: rotation angles along each axis  */
void Field::mirror(const bool isUseAll, const bool* planes,
                   const double* offset, const double* rotation) {
    /*  define transformer  */
    //  mirror that is parallel to which plane
    transform = vtkTransform::New();
    transform->Scale(planes[1] ? -1.0 : 1.0,  //
                     planes[2] ? -1.0 : 1.0,  //
                     planes[0] ? -1.0 : 1.0);
    //  translate from the plane
    transform->Translate(offset[0] * 2.0,  //
                         offset[1] * 2.0,  //
                         offset[2] * 2.0);
    //  rotation along axis
    transform->RotateX(rotation[0]);
    transform->RotateY(rotation[1]);
    transform->RotateZ(rotation[2]);

    /*  copy the original values  */
    transformFilter = vtkTransformFilter::New();
    vtkAlgorithmOutput* sourcePort;
    if (isUseAll) {
        sourcePort = denFilter->GetOutputPort();
    } else {
        sourcePort = portCur;
    }
    transformFilter->SetInputConnection(sourcePort);
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    /*  merge the ugrid  */
    appendFilter = vtkAppendFilter::New();
    appendFilter->AddInputConnection(sourcePort);
    appendFilter->AddInputConnection(transformFilter->GetOutputPort());
    appendFilter->Update();

    /*  clean the duplicated data  */
    cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
    cleanFilter->Update();

    /*  update the port and data in current  */
    ugridCur = cleanFilter->GetOutput();
    portCur  = cleanFilter->GetOutputPort();

    /*  release the temporary variables  */
    transform->Delete();
    transformFilter->Delete();
}

/*  getMirrorOutput: get the ugrid after mirror operation
 *  @return  the unstrictured grid after mirror operation  */
vtkUnstructuredGrid* Field::getMirrorOutput() {
    return cleanFilter->GetOutput();
}

/*  getMirrorOutput: get the data port after mirror operation
 *  @return  the data port after mirror operation  */
vtkAlgorithmOutput* Field::getMirrorOutputPort() {
    return cleanFilter->GetOutputPort();
}

/*  ============================================================================
 *  getCellDataArray: get the array amoung the all cell data
 *  @param  idx: the index of the cell data array  */
vtkDataArray* Field::getCellDataArray(const int& idx) {
    return cellData->GetArray(idx);
}

/*  ############################################################################
 *  assignFieldNameList: determine the list of the conbo box in viewerport */
void Field::assignFieldNameList() {
    /*  get the list of the point data  */
    fieldName.clear();
    for (int i = 0; i < pointData->GetNumberOfArrays(); ++i) {
        fieldNameList << pointData->GetArrayName(i);
    }

    /*  get the list of the cell data  */
    for (int i = 0; i < cellData->GetNumberOfArrays(); ++i) {
        fieldNameList << cellData->GetArrayName(i);
    }

    /*  determine the component name  */
    compName.clear();
    compNameList << "X"
                 << "Y"
                 << "Z"
                 << "Magnitude";
}
