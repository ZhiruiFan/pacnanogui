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

    /*  create the variables  */
    regionFilter = vtkAppendFilter::New();
    extractGeo   = vtkExtractGeometry::New();
    selectMap    = vtkDataSetMapper::New();
    selectActor  = vtkActor::New();
    selectActor->GetProperty()->SetColor(
        colors->GetColor3d("Tomato").GetData());
    frustum = vtkImplicitBoolean::New();
    planes  = vtkPlanes::New();

    /*  picker  */
    nodeFilter = vtkVertexGlyphFilter::New();

    /*  cell picker  */
    areaPicker    = vtkAreaPicker::New();
    cellPicker    = vtkCellPicker::New();
    cellExtracted = vtkUnstructuredGrid::New();
    nodeSelector  = vtkSelectionNode::New();
    cellSelector  = vtkSelection::New();
    extractor     = vtkExtractSelection::New();
    idFilter      = vtkIdFilter::New();
    locator       = vtkCellLocator::New();
    //  configurations
    numCellExtracted = 0;
    cellPicker->SetTolerance(0.001);
    nodeSelector->SetFieldType(vtkSelectionNode::CELL);
    nodeSelector->SetContentType(vtkSelectionNode::INDICES);

    /*  activate the selection mode  */
    CurrentMode = 1;
    /*  turn off the picking operation  */
    isActivated = false;
    cellIds     = vtkIdTypeArray::New();
};

/*  ############################################################################
 *  setInputData: assign the poly data to the current object
 *  @param  input: the field that will be operated  */
void Pick::setInputData(Field* input) {
    /*  assign the filed variables  */
    field = input;

    /*  initialize the id filter  */
    idFilter->SetInputData(field->ugrid);
    idFilter->SetCellIdsArrayName("All_cells");
    idFilter->SetPointIdsArrayName("All_nodes");
    idFilter->Update();

    /*  initialize the cell locator  */
    locator->SetDataSet(idFilter->GetOutput());
    locator->BuildLocator();

    /*  set the input of the geometry extractor  */
    extractGeo->SetInputData(idFilter->GetOutput());

    /*  set the input of the cell extractor  */
    extractor->SetInputConnection(0, idFilter->GetOutputPort());
}

/*  ============================================================================
 *  setRenderInfo: set the render window and renderer */
void Pick::setRenderInfo(vtkRenderer* renderer) {
    /*  assign renderer information  */
    ren = renderer;

    /*  assign the picker information  */
    areaPicker->Delete();
    areaPicker = vtkAreaPicker::New();
    ren->GetRenderWindow()->GetInteractor()->SetPicker(areaPicker);

    /*  reset the */

    /*  reset the frustum information  */
    frustum->Delete();
    frustum = vtkImplicitBoolean::New();

    /*  add the actor to the render  */
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
            /*  using the single selection mode  */
            if (StartPosition[0] == EndPosition[0]) {
                onCellSingleSelection();
            }
            /*  using the region selection mode  */
            else {
                onCellRegionSelection();
            }
        }
        //  Point selection mode
        else {
            onPointRegionSelection();
        }

        /*  configuration of the actor  */

        /*  render the window  */
    }
}

/*  ############################################################################
 *  OnRightButtonUp: override the event for the right button up, i.e.,
 *  reset the selected components */
void Pick::OnRightButtonUp() {
    vtkInteractorStyleRubberBandPick::OnRightButtonUp();

    //    /*  get the current posion of the mouse curser  */
    //    cellPicker->Pick(this->Interactor->GetEventPosition()[0],
    //                     this->Interactor->GetEventPosition()[1], 0, ren);

    //    /*  get the closest cell near the curser  */
    //    vtkIdType cellId = cellPicker->GetCellId();
    //    std::cout << "Cell ID: " << cellPicker->GetCellId() << std::endl;

    //    if (cellId >= 0) {
    //        cellIds->InsertNextValue(cellId);

    //        clickPoint->SetSelectionList(cellIds);

    //        clickCell->AddNode(clickPoint);

    //        singleSelect->SetInputData(1, clickCell);
    //        singleSelect->Update();

    //        vtkNew<vtkUnstructuredGrid> selected;
    //        selected->ShallowCopy(singleSelect->GetOutput());

    //        vtkNew<vtkDataSetMapper> selectedMapper;
    //        selectedMapper->SetInputData(selected);

    //        vtkNew<vtkActor> selectedActor;
    //        vtkNew<vtkNamedColors> colors;
    //        selectActor->SetMapper(selectedMapper);
    //        selectActor->GetProperty()->SetColor(
    //            colors->GetColor3d("Tomato").GetData());
    //        selectActor->GetProperty()->SetRepresentationToWireframe();

    //        ren->AddActor(selectedActor);
    //        ren->GetRenderWindow()->Render();
    //        HighlightProp(NULL);
    //    }
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
    // /* extract the new vtkPlanes   */
    // vtkPlanes* newPlanes = vtkPlanes::New();
    // newPlanes->SetNormals(areaPicker->GetFrustum()->GetNormals());
    // newPlanes->SetPoints(areaPicker->GetFrustum()->GetPoints());

    // /*  add the new plane to the implicit boolean  */
    // frustum->AddFunction(newPlanes);
    // frustum->SetOperationTypeToUnion();

    // /*  extract the geometry by using union planes  */
    // extractGeo->ExtractInsideOn();
    // extractGeo->SetImplicitFunction(frustum);
    // extractGeo->Update();
    // std::cout << extractGeo->GetOutput()->GetNumberOfCells() << std::endl;
    // /*  set the selected ugrid to the actor  */
    // selectMap->SetInputData(extractGeo->GetOutput());
    // selectActor->SetMapper(selectMap);

    // /*  configure the renderer property  */
    // selectActor->GetProperty()->SetColor(
    //     colors->GetColor3d("Tomato").GetData());
    // selectActor->GetProperty()->SetRepresentationToWireframe();
    // selectActor->GetProperty()->SetLineWidth(4.0);

    // /*  assign the actor to the renderer  */
    // ren->AddActor(selectActor);
    // Interactor->GetRenderWindow()->Render();
    // HighlightProp(NULL);
    //    extractGeo->SetInputConnection(idFilter->GetOutputPort());
    extractGeo->SetInputData(idFilter->GetOutput());
    extractGeo->ExtractInsideOn();
    extractGeo->SetImplicitFunction(areaPicker->GetFrustum());
    extractGeo->Update();

    /*  extract the ids of cells in extracted geometry  */
    //  get the number of cells
    vtkIdType num = extractGeo->GetOutput()->GetNumberOfCells();
    if (num > 0) {
        vtkPoints* points;
        double point[3];
        for (vtkIdType i = 0; i < num; ++i) {
            //  get the points of the current cell
            points = extractGeo->GetOutput()->GetCell(i)->GetPoints();
            double cellCenter[3] = {0.0, 0.0, 0.0};
            vtkIdType numPoints  = points->GetNumberOfPoints();
            for (vtkIdType pointId = 0; pointId < numPoints; ++pointId) {
                points->GetPoint(pointId, point);
                cellCenter[0] += point[0];
                cellCenter[1] += point[1];
                cellCenter[2] += point[2];
            }
            cellCenter[0] /= numPoints;
            cellCenter[1] /= numPoints;
            cellCenter[2] /= numPoints;
            //  find the cell id
            locator->SetTolerance(0.001);
            cellIds->InsertNextValue(locator->FindCell(cellCenter));
        }

        /*  display the selected cells  */
        showSelectedCells();
    }
}

/*  ============================================================================
 *  onCellSingleSelection: preform the single selection when the piking
 *  mode is actived.  */
void Pick::onCellSingleSelection() {
    /*  perform the single cell picking  */
    cellPicker->Pick(StartPosition[0], StartPosition[1], 0, ren);
    /*  extract the picked cell  */
    if (cellPicker->GetCellId() >= 0) {
        //  append the cell id to the list
        cellIds->InsertNextValue(cellPicker->GetCellId());
        //  show the extracted cells
        showSelectedCells();
    }
}

/*  ============================================================================
 *  showSelectedCells: display the selected cells to the render window  */
void Pick::showSelectedCells() {
    /*  extract the newly added cells   */
    nodeSelector->SetSelectionList(cellIds);
    cellSelector->AddNode(nodeSelector);
    extractor->SetInputData(1, cellSelector);
    extractor->Update();
    cellExtracted->ShallowCopy(extractor->GetOutput());

    /*  assign the extracted cells to the mapper  */
    selectMap->SetInputData(cellExtracted);
    selectActor->SetMapper(selectMap);

    /*  configure the renderer property  */
    selectActor->GetProperty()->SetColor(
        colors->GetColor3d("Tomato").GetData());
    selectActor->GetProperty()->SetRepresentationToWireframe();
    selectActor->GetProperty()->SetLineWidth(4.0);

    /*  display the extracted cells  */
    Interactor->GetRenderWindow()->Render();
    HighlightProp(NULL);
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
