#include "add_object_dialog.h"
#include "./ui_add_object_dialog.h"
#include "main_window.h"

AddObjectDialog::AddObjectDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddObjectDialog), m_window(nullptr) {
    ui->setupUi(this);
}

AddObjectDialog::~AddObjectDialog() { delete ui; }

void AddObjectDialog::setupConnections(MainWindow *window) {
    m_window = window;

    QObject::connect(
        ui->pushButtonTorus, &QPushButton::clicked, this,
        &AddObjectDialog::requestAddTorus
    );

    QObject::connect(
        ui->pushButtonPoint, &QPushButton::clicked, this,
        &AddObjectDialog::requestAddPoint
    );
}

void AddObjectDialog::requestAddTorus() {
    m_window->add(ObjectType::TorusObject);
    accept();
}

void AddObjectDialog::requestAddPoint() {
    m_window->add(ObjectType::PointObject);
    accept();
}
