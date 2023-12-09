#ifndef MATASSIGN_H
#define MATASSIGN_H

#include <QDialog>

namespace Ui {
class MatAssign;
}

class MatAssign : public QDialog
{
    Q_OBJECT

public:
    explicit MatAssign(QWidget *parent = nullptr);
    ~MatAssign();

private:
    Ui::MatAssign *ui;
};

#endif // MATASSIGN_H
