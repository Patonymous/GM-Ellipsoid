#include "position_params.h"
#include "../renderable.h"
#include "./ui_position_params.h"

PositionParams::PositionParams(QWidget *parent)
    : QWidget(parent), ui(new Ui::PositionParams) {
    ui->setupUi(this);
}

PositionParams::~PositionParams() { delete ui; }

void PositionParams::setupConnections(IRenderable *renderable) {
    ui->spinBoxX->setValue(renderable->model().position.x);
    QObject::connect(
        ui->spinBoxX, &QDoubleSpinBox::valueChanged, renderable,
        &IRenderable::setPositionX
    );
    QObject::connect(
        renderable, &IRenderable::positionXChanged, ui->spinBoxX,
        &QDoubleSpinBox::setValue
    );

    ui->spinBoxY->setValue(renderable->model().position.y);
    QObject::connect(
        ui->spinBoxY, &QDoubleSpinBox::valueChanged, renderable,
        &IRenderable::setPositionY
    );
    QObject::connect(
        renderable, &IRenderable::positionYChanged, ui->spinBoxY,
        &QDoubleSpinBox::setValue
    );

    ui->spinBoxZ->setValue(renderable->model().position.z);
    QObject::connect(
        ui->spinBoxZ, &QDoubleSpinBox::valueChanged, renderable,
        &IRenderable::setPositionZ
    );
    QObject::connect(
        renderable, &IRenderable::positionZChanged, ui->spinBoxZ,
        &QDoubleSpinBox::setValue
    );
}
