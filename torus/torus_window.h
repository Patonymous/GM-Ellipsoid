#ifndef TORUS_WINDOW_H
#define TORUS_WINDOW_H

#include <QMainWindow>

#include "torus.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class TorusWindow;
}
QT_END_NAMESPACE

class TorusWindow : public QMainWindow {
    Q_OBJECT

public:
    TorusWindow(QWidget *parent = nullptr);
    ~TorusWindow();

private:
    Ui::TorusWindow *ui;

    Torus m_torus;
};

#endif // TORUS_WINDOW_H
