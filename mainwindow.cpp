#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QObject::connect(
        ui->spinBoxX, &QDoubleSpinBox::valueChanged, ui->openGLWidget,
        &Ellipsoid::setStretchX
    );

    QObject::connect(
        ui->spinBoxY, &QDoubleSpinBox::valueChanged, ui->openGLWidget,
        &Ellipsoid::setStretchY
    );

    QObject::connect(
        ui->spinBoxZ, &QDoubleSpinBox::valueChanged, ui->openGLWidget,
        &Ellipsoid::setStretchZ
    );
}

MainWindow::~MainWindow() { delete ui; }
