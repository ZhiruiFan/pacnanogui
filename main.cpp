#include <QVTKOpenGLStereoWidget.h>
#include <QVTKOpenGLWindow.h>

#include <QApplication>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "pacnano.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    pacnano w;
    w.show();
    return a.exec();
}
