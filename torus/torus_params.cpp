#include "torus_params.h"
#include "./ui_torus_params.h"
#include "torus.h"

TorusParams::TorusParams(QWidget *parent)
    : QWidget(parent), ui(new Ui::TorusParams) {
    ui->setupUi(this);
}

TorusParams::~TorusParams() { delete ui; }

void TorusParams::setupConnections(Torus *torus) {

    ui->spinBoxT->setValue(torus->tSamples());
    QObject::connect(
        ui->spinBoxT, &QSpinBox::valueChanged, torus, &Torus::setTSamples
    );

    ui->spinBoxS->setValue(torus->sSamples());
    QObject::connect(
        ui->spinBoxS, &QSpinBox::valueChanged, torus, &Torus::setSSamples
    );

    ui->spinBoxBig->setValue(torus->bigRadius());
    QObject::connect(
        ui->spinBoxBig, &QDoubleSpinBox::valueChanged, torus,
        &Torus::setBigRadius
    );

    ui->spinBoxSmall->setValue(torus->smallRadius());
    QObject::connect(
        ui->spinBoxSmall, &QDoubleSpinBox::valueChanged, torus,
        &Torus::setSmallRadius
    );
}
