#include "pacnano.h"
#include <QApplication>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <vtk-9.3/vtkRenderWindow.h>
#include <QVTKOpenGLWindow.h>
#include <QVTKOpenGLStereoWidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    pacnano w;
    w.show();
    return a.exec();
}
