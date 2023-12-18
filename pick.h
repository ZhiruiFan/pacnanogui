/*  ============================================================================
 *
 *       _ __   __ _  ___ _ __   __ _ _ __   ___
 *      | '_ \ / _` |/ __| '_ \ / _` | '_ \ / _ \
 *      | |_) | (_| | (__| | | | (_| | | | | (_) |
 *      | .__/ \__,_|\___|_| |_|\__,_|_| |_|\___/
 *      |_|
 *
 *      File name  : pick.h
 *      Version    : 3.0
 *      Author     : Jerry Fan
 *      Date       : December 15th, 2023
 *      All copyright © is reserved by zhirui.fan
 *  ============================================================================
 *  */
#ifndef PICK_H
#define PICK_H
/*  HEAD FILES FOR VTK  */
#include <vtkActor.h>
#include <vtkAppendFilter.h>
#include <vtkAreaPicker.h>
#include <vtkCellLocator.h>
#include <vtkCellPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractSelection.h>
#include <vtkIdFilter.h>
#include <vtkImplicitBoolean.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkPlaneCollection.h>
#include <vtkPlanes.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkVertexGlyphFilter.h>

#include <QObject>
#include <QWidget>

#include "field.h"

/*  ############################################################################
 *  CLASS Pick: this class is used to define the picking process of elements
 *      and points. Up to now, two selecting method, i.e., region selection and
 *      cliking/single selection is defined.  */
class Pick : public vtkInteractorStyleRubberBandPick {
private:
    bool mode;                           // node or cell mode
    bool isActivated;                    // status flag

    vtkRenderer* ren;                    // renderer of the window
    vtkNamedColors* colors;              // buid-in color

    vtkActor* selectActor;               // actor for selection
    vtkDataSetMapper* selectMap;         // mappler of data selection
    vtkPlanes* planes;
    vtkImplicitBoolean* frustum;         // viewerport frustum

    vtkAppendFilter* regionFilter;       // append the region selection

    vtkVertexGlyphFilter* nodeFilter;    // nodal filter
    Field* field;                        // current operated data

    vtkCellPicker* cellPicker;           // cell picker
    vtkAreaPicker* areaPicker;           // area picker
    vtkIdFilter* idFilter;               // id filters
    vtkExtractGeometry* extractGeo;      // model clip handler
    vtkSelectionNode* nodeSelector;      // node selector
    vtkSelection* cellSelector;          // cell selector
    vtkExtractSelection* extractor;      // cell extractor
    vtkCellLocator* locator;             // cell locator
    vtkIdTypeArray* cellIds;             // extracted cells
    vtkUnstructuredGrid* cellExtracted;  // the extracted cells

public:
    /*  New: create the object using the VTK style  */
    static Pick* New();
    vtkTypeMacro(Pick, vtkInteractorStyleRubberBandPick);

    /*  Constructor: create the Pick object  */
    Pick();

    /*  setActivate: set the activate status for the picker  */
    void setActivateStatus(const bool& status) { isActivated = status; }

    /*  setCellSelectMode: set the selection mode to cells  */
    void setCellSelectMode() { mode = true; }

    /*  setPointSelectMode: set the selection model to points  */
    void setPointSelectMode() { mode = false; }

    /*  setField: assign field variables to the current viewer object
     *  @param  input: the field that will be operated  */
    void setField(Field* input);

    /*  setInputData: assign the unstructured grid data to the current object
     *  @param  input: the unstructured grid be operated  */
    void setInputData(vtkUnstructuredGrid* input);

    /*  setRenderInfo: set the render window and renderer
     *  @param  renderWindow: the window to show the model
     *  @param  renderer: the rendered object  */
    void setRenderInfo(vtkRenderer* renderer);

public:
    /*  ########################################################################
     *  OnLeftButtonUp: override the event for the left button up, i.e.,
     *  define the event when the picking operation is completed  */
    virtual void OnLeftButtonUp() override;

    /*  OnMouseMove: override the event for the mouse movement, i.e.,
     *  highlighten the neareast cells or nodes  */
    virtual void OnMouseMove() override;

    /*  OnRightButtonUp: override the event for the right button up, i.e.,
     *  reset the selected components */
    virtual void OnRightButtonUp() override;

    /*  turnOff: turn off the selection mode, i.e., remove the selection
     *      actor from the render window  */
    void turnOff();

private:
    /*  ########################################################################
     *  onCellRegionSelection: preform the region selection when the piking
     *  mode is actived.  */
    void onCellRegionSelection();

    /*  onCellSingleSelection: preform the single selection when the piking
     *  mode is actived.  */
    void onCellSingleSelection();

    /*  onPointRegionSelection: preform the region point selection when the
     *  picking mode is actived.  */
    void onPointRegionSelection();

    /*  onPointSingleSelection: preform the single point selection when the
     *  picking mode is actived.  */
    void onPointSingleSelection();

    /*  showSelectedCells: display the selected cells to the render window  */
    void showSelectedCells();

public:
    /*  ########################################################################
     *  isPickerActivated: return the status of the picker where whether the
     *      actor is activated.
     *   @return  if the actor is actived  */
    bool isPickerActivated() { return isActivated; }

    /*  isCellModeOn: get the status of the picker that if the cell
     *      selection model is actived
     *  @return  the selection mode, true for element, false for node  */
    bool isCellSelectionModeOn() { return mode; }

    /*  getFrustum: get the frustum or vtk planes of the area picker  */
    vtkImplicitBoolean* getFrustum() { return frustum; }

    /*  getSelectedCellIds: return the id set of the selected cells
     *  @return  the selected cell ids  */
    vtkIdTypeArray* getSelectedCellIds() { return cellIds; }

    /*  getIdFilterPort: get the idFilter with respect to the current field
     *  variables
     *  @return   the port of the idFilter  */
    vtkIdFilter* getIdFilter() { return idFilter; }

    /*  getCellLocator: get the cell locator to quirey the cell using centroid
     *  location
     *  @return  the cell locator  */
    vtkCellLocator* getCellLocator() { return locator; }
};

#endif  // PICK_H
