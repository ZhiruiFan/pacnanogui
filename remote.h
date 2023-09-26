#ifndef REMOTE_H
#define REMOTE_H

#include <QDialog>

namespace Ui {
class Remote;
}

class Remote : public QDialog {
    Q_OBJECT

public:
    explicit Remote(QWidget *parent = nullptr);
    ~Remote();

public slots:

private:
    Ui::Remote *ui;
};

#endif  // REMOTE_H
