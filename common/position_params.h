#ifndef POSITION_PARAMS_H
#define POSITION_PARAMS_H

#include <QWidget>

class IRenderable;

QT_BEGIN_NAMESPACE
namespace Ui {
    class PositionParams;
}
QT_END_NAMESPACE

class PositionParams : public QWidget {
    Q_OBJECT

public:
    PositionParams(QWidget *parent = nullptr);
    ~PositionParams();

    void setupConnections(IRenderable *renderable);

private:
    Ui::PositionParams *ui;
};

#endif // POSITION_PARAMS_H
