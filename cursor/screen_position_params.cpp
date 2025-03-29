#include <QPushButton>

#include "./ui_screen_position_params.h"
#include "cursor.h"
#include "screen_position_params.h"

ScreenPositionParams::ScreenPositionParams(QWidget *parent)
    : QWidget(parent), ui(new Ui::ScreenPositionParams), m_dialog() {
    ui->setupUi(this);
}

ScreenPositionParams::~ScreenPositionParams() { delete ui; }

void ScreenPositionParams::setupConnections(Cursor *cursor) {
    updateScreenPositionLabels(cursor->screenX(), cursor->screenY());
    QObject::connect(
        cursor, &Cursor::screenPositionChanged, this,
        &ScreenPositionParams::updateScreenPositionLabels
    );

    QObject::connect(
        ui->pushButtonSet, &QPushButton::clicked, &m_dialog, &QDialog::open
    );

    m_dialog.setupConnections(cursor);
}

void ScreenPositionParams::updateScreenPositionLabels(float x, float y) {
    ui->labelXValue->setText(QString::number(x, 'f', 2));
    ui->labelYValue->setText(QString::number(y, 'f', 2));
}
