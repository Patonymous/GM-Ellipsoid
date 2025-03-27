#ifndef TORUS_PARAMS_H
#define TORUS_PARAMS_H

#include <QWidget>

class Torus;

QT_BEGIN_NAMESPACE
namespace Ui {
    class TorusParams;
}
QT_END_NAMESPACE

class TorusParams : public QWidget {
    Q_OBJECT

public:
    TorusParams(QWidget *parent = nullptr);
    ~TorusParams();

    void setupConnections(Torus *torus);

private:
    Ui::TorusParams *ui;
};

#endif // TORUS_PARAMS_H
