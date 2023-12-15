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
#include <vtkCellPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractSelection.h>
#include <vtkImplicitBoolean.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkPlanes.h>
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

/*  CLASS Pick: this class is used to define the picking process of elements
 *      and points. Up to now, two selecting method, i.e., region selection and
 *      cliking/single selection is defined.  */
class Pick : public vtkInteractorStyleRubberBandPick {
private:
    bool mode;                          // node or cell mode
    bool isActivated;                   // status flag

    vtkRenderer* ren;                   // renderer of the window
    vtkNamedColors* colors;             // buid-in color

    vtkActor* selectActor;              // actor for selection
    vtkDataSetMapper* selectMap;        // mappler of data selection
    vtkImplicitBoolean* frustum;        // viewerport frustum
    vtkAreaPicker* areaPicker;          // area picker
    vtkCellPicker* cellPicker;          // cell picker

    vtkExtractSelection* regionSelect;  // region selection handler
    vtkExtractGeometry* extractGeo;     // model clip handler
    vtkAppendFilter* regionFilter;      // append the region selection

    vtkVertexGlyphFilter* nodeFilter;   // nodal filter
    Field* field;                       // current operated data

    vtkIdType cellId;                   // Id for the picked cell
    vtkIdTypeArray* cellIds;            // array to store the cllicked cell
    vtkSelectionNode* clickPoint;       // mouse selection point
    vtkSelection* clickCell;            // selected cell by clicking
    vtkExtractSelection* singleSelect;  // single selection handler
    vtkCell* pickedCell;
    unsigned int numCells;              // number of picked cells
    vtkUnstructuredGrid* ugridClick;    // clicked ugrid
    vtkUnstructuredGrid* ugridRegion;   // the all ugrid
    vtkAppendFilter* appendFilter;      // append the ugrid

public:
    /*  New: create the object using the VTK style  */
    static Pick* New();
    vtkTypeMacro(Pick, vtkInteractorStyleRubberBandPick);
    /*  Constructor: create the Pick object  */
    Pick();

    /*  OnLeftButtonUp: override the event for the left button up, i.e.,
     *  define the event when the picking operation is completed  */
    virtual void OnLeftButtonUp() override;

    /*  OnMouseMove: override the event for the mouse movement, i.e.,
     *  highlighten the neareast cells or nodes  */
    virtual void OnMouseMove() override;

    /*  OnRightButtonUp: override the event for the right button up, i.e.,
     *  reset the selected components */
    virtual void OnRightButtonUp() override;

    /*  setPolyData: assign the poly data to the current object
     *  @param  input: the field that will be operated  */
    void setInputData(Field* input);

    /*  setCellSelectMode: set the selection mode to cells  */
    void setCellSelectMode() { mode = true; }
    /*  setPointSelectMode: set the selection model to points  */
    void setPointSelectMode() { mode = false; }

    /*  setRenderInfo: set the render window and renderer
     *  @param  renderWindow: the window to show the model
     *  @param  renderer: the rendered object  */
    void setRenderInfo(vtkRenderer* renderer);

    /*  turnOff: turn off the selection mode, i.e., remove the selection
     *      actor from the render window  */
    void turnOff();

    /*  isPickerActivated: return the status of the picker where whether the
     *      actor is activated.
     *   @return  if the actor is actived  */
    bool isPickerActivated() { return isActivated; }

    /*  isCellModeOn: get the status of the picker that if the cell
     *      selection model is actived
     *  @return  the selection mode, true for element, false for node  */
    bool isCellSelectionModeOn() { return mode; }

    /*  getFrustum: get the frustum or vtk planes of the area picker  */
    vtkPlanes* getFrustum() { return areaPicker->GetFrustum(); }

private:
    /*  onCellRegionSelection: preform the region selection when the piking
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
};

#endif  // PICK_H
