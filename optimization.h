#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <QDialog>

namespace Ui {
class Optimization;
}

class Optimization : public QDialog
{
    Q_OBJECT

public:
    explicit Optimization(QWidget *parent = nullptr);
    ~Optimization();

private:
    Ui::Optimization *ui;
};

#endif // OPTIMIZATION_H
