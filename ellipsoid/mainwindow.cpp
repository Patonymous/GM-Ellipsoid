#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    const auto &ellipsoid = *ui->ellipsoid;
    const auto  params    = ellipsoid.currentParams();

    ui->spinBoxX->setValue(params.stretchX);
    QObject::connect(
        ui->spinBoxX, &QDoubleSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setStretchX
    );

    ui->spinBoxY->setValue(params.stretchY);
    QObject::connect(
        ui->spinBoxY, &QDoubleSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setStretchY
    );

    ui->spinBoxZ->setValue(params.stretchZ);
    QObject::connect(
        ui->spinBoxZ, &QDoubleSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setStretchZ
    );

    ui->spinBoxAmbient->setValue(params.lightAmbient);
    QObject::connect(
        ui->spinBoxAmbient, &QDoubleSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setLightAmbient
    );

    ui->spinBoxDiffuse->setValue(params.lightDiffuse);
    QObject::connect(
        ui->spinBoxDiffuse, &QDoubleSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setLightDiffuse
    );

    ui->spinBoxSpecular->setValue(params.lightSpecular);
    QObject::connect(
        ui->spinBoxSpecular, &QDoubleSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setLightSpecular
    );

    ui->spinBoxFocus->setValue(params.lightSpecularFocus);
    QObject::connect(
        ui->spinBoxFocus, &QDoubleSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setLightSpecularFocus
    );

    ui->spinBoxScale->setValue(params.scale);
    QObject::connect(
        ui->spinBoxScale, &QDoubleSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setScale
    );

    ui->spinBoxGranularity->setValue(ellipsoid.initialPixelGranularity());
    QObject::connect(
        ui->spinBoxGranularity, &QSpinBox::valueChanged, ui->ellipsoid,
        &Ellipsoid::setInitialPixelGranularity
    );
}

MainWindow::~MainWindow() { delete ui; }
