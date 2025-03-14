#ifndef TORUS_WINDOW_H
#define TORUS_WINDOW_H

#include <QMainWindow>

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
};

#endif // TORUS_WINDOW_H
