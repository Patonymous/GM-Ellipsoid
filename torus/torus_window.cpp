#include "torus_window.h"
#include "./ui_torus_window.h"

TorusWindow::TorusWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TorusWindow) {
    ui->setupUi(this);
}

TorusWindow::~TorusWindow() { delete ui; }
