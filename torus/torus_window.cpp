#include "torus_window.h"
#include "./ui_torus_window.h"

TorusWindow::TorusWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::TorusWindow), m_torus() {
    ui->setupUi(this);

    ui->openGlArea->tryPlaceRenderable(&m_torus);
    ui->openGlArea->setActive(&m_torus);

    ui->spinBoxT->setValue(m_torus.tSamples());
    QObject::connect(
        ui->spinBoxT, &QSpinBox::valueChanged, &m_torus, &Torus::setTSamples
    );

    ui->spinBoxS->setValue(m_torus.sSamples());
    QObject::connect(
        ui->spinBoxS, &QSpinBox::valueChanged, &m_torus, &Torus::setSSamples
    );
}

TorusWindow::~TorusWindow() { delete ui; }
