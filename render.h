#ifndef RENDER_H
#define RENDER_H

#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellType.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkUnstructuredGrid.h>

#include <QObject>
#include <QWidget>

class Render : public QWidget {
    Q_OBJECT

private:
    vtkNamedColors* colors;
    vtkRenderer* renderer;
    vtkRenderWindow* renWin;
    QVTKOpenGLNativeWidget* win;
    vtkRenderWindowInteractor* iren;
    vtkPoints* points;
    vtkUnstructuredGrid* ugrid;
    vtkDataSetMapper* ugridMapper;
    vtkActor* actor;

public:
    /*  constructor: create the render window to show the FEM model
     *  @param  window: the openGL widget   */
    Render(QVTKOpenGLNativeWidget* window);
};

#endif  // RENDER_H
