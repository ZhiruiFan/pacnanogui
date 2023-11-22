#include "optimization.h"
#include "ui_optimization.h"

Optimization::Optimization(QWidget *parent) :
                                              QDialog(parent),
                                              ui(new Ui::Optimization)
{
    ui->setupUi(this);
}

Optimization::~Optimization()
{
    delete ui;
}
