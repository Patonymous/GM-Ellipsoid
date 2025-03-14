#ifndef ELLIPSOID_WINDOW_H
#define ELLIPSOID_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class EllipsoidWindow;
}
QT_END_NAMESPACE

class EllipsoidWindow : public QMainWindow {
    Q_OBJECT

public:
    EllipsoidWindow(QWidget *parent = nullptr);
    ~EllipsoidWindow();

private:
    Ui::EllipsoidWindow *ui;
};

#endif // ELLIPSOID_WINDOW_H
