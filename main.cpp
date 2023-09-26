#include "pacnano.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    pacnano w;
    w.show();
    return a.exec();
}
