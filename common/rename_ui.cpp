#include <QPlainTextEdit>

#include "../renderable.h"
#include "./ui_rename_ui.h"
#include "rename_ui.h"

RenameUi::RenameUi(QWidget *parent) : QWidget(parent), ui(new Ui::RenameUi) {
    ui->setupUi(this);
}

RenameUi::~RenameUi() { delete ui; }

void RenameUi::setupConnections(IRenderable *renderable) {
    m_renderable = renderable;

    ui->plainTextEditName->setPlainText(renderable->name());
    QObject::connect(
        ui->plainTextEditName, &QPlainTextEdit::textChanged, this,
        &RenameUi::updateName
    );
}

void RenameUi::updateName() {
    m_renderable->setName(ui->plainTextEditName->toPlainText());
}
