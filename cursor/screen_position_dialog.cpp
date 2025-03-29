#include "screen_position_dialog.h"
#include "./ui_screen_position_dialog.h"
#include "cursor.h"

ScreenPositionDialog::ScreenPositionDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ScreenPositionDialog), m_cursor(nullptr) {
    ui->setupUi(this);
}

ScreenPositionDialog::~ScreenPositionDialog() { delete ui; }

void ScreenPositionDialog::setupConnections(Cursor *cursor) {
    m_cursor = cursor;

    QObject::connect(
        this, &QDialog::accepted, this,
        &ScreenPositionDialog::requestScreenPosition
    );
}

void ScreenPositionDialog::requestScreenPosition() {
    m_cursor->requestScreenPosition(
        ui->spinBoxX->value(), ui->spinBoxY->value()
    );
}
