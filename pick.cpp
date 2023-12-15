/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : pick.cpp
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : December 15th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#include "pick.h"

/* #############################################################################
 *  New: define the New function using the built-in interface of VTK  */
vtkStandardNewMacro(Pick);

/*  ============================================================================
 *  Constructor: create the Pick object  */
Pick::Pick() : vtkInteractorStyleRubberBandPick() {
    /*  Colors object  */
    colors = vtkNamedColors::New();

    /*  the all of unstructured grid  */
    ugridClick   = vtkUnstructuredGrid::New();
    appendFilter = vtkAppendFilter::New();

    /*  create the variables  */
    regionSelect = vtkExtractSelection::New();
    regionFilter = vtkAppendFilter::New();
    ugridRegion  = vtkUnstructuredGrid::New();
    extractGeo   = vtkExtractGeometry::New();
    selectMap    = vtkDataSetMapper::New();
    selectActor  = vtkActor::New();
    selectActor->SetMapper(selectMap);
    selectActor->GetProperty()->SetColor(
        colors->GetColor3d("Tomato").GetData());
    frustum = vtkImplicitBoolean::New();

    /*  picker  */
    areaPicker = vtkAreaPicker::New();
    nodeFilter = vtkVertexGlyphFilter::New();

    /*  cell picker  */
    cellPicker = vtkCellPicker::New();
    cellPicker->SetTolerance(0.001);
    clickPoint = vtkSelectionNode::New();
    clickPoint->SetFieldType(vtkSelectionNode::CELL);
    clickPoint->SetContentType(vtkSelectionNode::INDICES);
    clickCell    = vtkSelection::New();
    cellIds      = vtkIdTypeArray::New();
    singleSelect = vtkExtractSelection::New();

    /*  activate the selection mode  */
    CurrentMode = 1;
    /*  turn off the picking operation  */
    isActivated = false;
};

/*  ============================================================================
 *  setPolyData: assign the poly data to the current object
 *  @param  input: the field that will be operated  */
void Pick::setInputData(Field* input) {
    field = input;
    extractGeo->SetInputData(input->ugrid);
    numCells = 0;
}

/*  ============================================================================
 *  setRenderInfo: set the render window and renderer */
void Pick::setRenderInfo(vtkRenderer* renderer) {
    /*  assign renderer information  */
    ren = renderer;
    //    areaPicker = picker;
    /*  add actor  */
    areaPicker->Delete();
    areaPicker = vtkAreaPicker::New();
    ren->GetRenderWindow()->GetInteractor()->SetPicker(areaPicker);
    ren->AddActor(selectActor);
    isActivated = true;
}

/*  ############################################################################
 *  OnLectButtonUp: the overrided member function to define the event when the
 *  left mouse button is up  */
void Pick::OnLeftButtonUp() {
    /*  perform the forward member function  */
    vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

    /*  show the selection actor  */
    isActivated = true;
    selectActor->VisibilityOn();

    /*  if the selection mode is actived  */
    if (CurrentMode == 1) {
        /*  configuration for the dataset mapper  */
        selectMap->ScalarVisibilityOff();
        //  Cell selection mode
        if (mode) {
            /*  determine the mode is region selection or point selection  */
            if (StartPosition[0] == EndPosition[0]) {
                //  perform the single selection
                onCellSingleSelection();
                appendFilter->AddInputData(ugridClick);
            } else {
                //  perform the region selection
                onCellRegionSelection();
                appendFilter->AddInputData(ugridRegion);
            }
            /*  add the selected cells to the ugrid  */
            //            appendFilter->AddInputData(extractGeo->GetOutput());
            appendFilter->Update();

            /*  set the selected ugrid to the actor  */
            selectMap->SetInputData(appendFilter->GetOutput());
            selectActor->SetMapper(selectMap);

            /*  configure the renderer property  */
            selectActor->GetProperty()->SetColor(
                colors->GetColor3d("Tomato").GetData());
            selectActor->GetProperty()->SetRepresentationToWireframe();

            /*  assign the actor to the renderer  */
            ren->AddActor(selectActor);
        }
        //  Point selection mode
        else {
            onPointRegionSelection();
        }

        /*  configuration of the actor  */

        /*  render the window  */
        GetInteractor()->GetRenderWindow()->Render();
        HighlightProp(NULL);
    }
}

/*  ############################################################################
 *  OnRightButtonUp: override the event for the right button up, i.e.,
 *  reset the selected components */
void Pick::OnRightButtonUp() {
    vtkInteractorStyleRubberBandPick::OnRightButtonUp();

    /*  get the current posion of the mouse curser  */
    cellPicker->Pick(this->Interactor->GetEventPosition()[0],
                     this->Interactor->GetEventPosition()[1], 0, ren);

    /*  get the closest cell near the curser  */
    vtkIdType cellId = cellPicker->GetCellId();
    std::cout << "Cell ID: " << cellPicker->GetCellId() << std::endl;

    if (cellId >= 0) {
        cellIds->InsertNextValue(cellId);

        clickPoint->SetSelectionList(cellIds);

        clickCell->AddNode(clickPoint);

        singleSelect->SetInputData(1, clickCell);
        singleSelect->Update();

        vtkNew<vtkUnstructuredGrid> selected;
        selected->ShallowCopy(singleSelect->GetOutput());

        vtkNew<vtkDataSetMapper> selectedMapper;
        selectedMapper->SetInputData(selected);

        vtkNew<vtkActor> selectedActor;
        vtkNew<vtkNamedColors> colors;
        selectActor->SetMapper(selectedMapper);
        selectActor->GetProperty()->SetColor(
            colors->GetColor3d("Tomato").GetData());
        selectActor->GetProperty()->SetRepresentationToWireframe();

        ren->AddActor(selectedActor);
        ren->GetRenderWindow()->Render();
        HighlightProp(NULL);
    }
}

/*  ############################################################################
 *  OnRightButtonUp: override the event for the right button up, i.e.,
 *  reset the selected components */
void Pick::OnMouseMove() {
    /*  execute the defalut event on the */
    vtkInteractorStyleRubberBandPick::OnMouseMove();
}

/*  ############################################################################
 *  onCellRegionSelection: preform the region selection when the piking
 *  mode is actived.  */
void Pick::onCellRegionSelection() {
    /*  get the selected unstructured grids  */
    frustum->AddFunction(areaPicker->GetFrustum());
    frustum->SetOperationTypeToUnion();
    extractGeo->SetImplicitFunction(frustum);
    extractGeo->Update();

    //  set the dataset mapper
    std::cout << "Extracted " << extractGeo->GetOutput()->GetNumberOfCells()
              << " cells." << std::endl;
    //  configuration of the actor displaying
    ugridRegion = extractGeo->GetOutput();
}

/*  ============================================================================
 *  onCellSingleSelection: preform the single selection when the piking
 *  mode is actived.  */
void Pick::onCellSingleSelection() {
    /*  get the current posion of the mouse curser  */
    cellPicker->Pick(this->Interactor->GetEventPosition()[0],
                     this->Interactor->GetEventPosition()[1], 0, ren);

    /*  get the closest cell near the curser  */
    cellId = cellPicker->GetCellId();
    std::cout << "Cell ID: " << cellPicker->GetCellId() << std::endl;

    /*  add the selected cell to the actor  */
    if (cellId >= 0) {
        /*  get the vtkCell from the cell id  */
        cellIds->InsertNextValue(cellId);
        clickPoint->SetSelectionList(cellIds);
        clickCell->AddNode(clickPoint);

        /*  assign the selected cells to the grid variables */
        singleSelect->SetInputConnection(0, field->port);
        singleSelect->SetInputData(1, clickCell);
        singleSelect->Update();
        ugridClick->ShallowCopy(singleSelect->GetOutput());
    }
}

/*  ============================================================================
 *  onPointRegionSelection: preform the region point selection when the
 *  picking mode is actived.  */
void Pick::onPointRegionSelection() {
    //  define the point filter
    nodeFilter->SetInputData(extractGeo->GetOutput());
    nodeFilter->Update();
    //  set the dataset mapper
    selectMap->SetInputData(nodeFilter->GetOutput());
    //  display the number of selected object
    std::cout << "Extracted " << extractGeo->GetOutput()->GetNumberOfPoints()
              << " Points." << std::endl;
    //  configuration for the actor displaying
    selectActor->GetProperty()->SetRepresentationToPoints();
    selectActor->GetProperty()->SetPointSize(5);
    selectActor->GetProperty()->SetVertexVisibility(true);
}

/*  onPointSingleSelection: preform the single point selection when the
 *  picking mode is actived.  */
void onPointSingleSelection();

/*  ===========================================================================
 *  turnOff: turn off the selection mode, i.e., remove the selection
 *           actor from the render window  */
void Pick::turnOff() {
    if (isActivated) {
        //  remove the selection actor
        selectActor->VisibilityOff();

        //  update the status flag
        isActivated = false;
    }
}
