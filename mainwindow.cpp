#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->openGLWidget->setStretchX(ui->spinBoxX->value());
    QObject::connect(
        ui->spinBoxX, &QDoubleSpinBox::valueChanged, ui->openGLWidget,
        &Ellipsoid::setStretchX
    );

    ui->openGLWidget->setStretchY(ui->spinBoxY->value());
    QObject::connect(
        ui->spinBoxY, &QDoubleSpinBox::valueChanged, ui->openGLWidget,
        &Ellipsoid::setStretchY
    );

    ui->openGLWidget->setStretchZ(ui->spinBoxZ->value());
    QObject::connect(
        ui->spinBoxZ, &QDoubleSpinBox::valueChanged, ui->openGLWidget,
        &Ellipsoid::setStretchZ
    );
}

MainWindow::~MainWindow() { delete ui; }
