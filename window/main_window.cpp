#include "main_window.h"
#include "./ui_main_window.h"

#include "../cursor/cursor.h"
#include "../point/point.h"
#include "../torus/torus.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_renderables(),
      m_addDialog(this) {
    ui->setupUi(this);
    m_addDialog.setupConnections(this);

    ui->labelProjectionDetails->setText(ui->openGlArea->projection());
    QObject::connect(
        ui->openGlArea, &OpenGLArea::projectionChanged,
        ui->labelProjectionDetails, &QLabel::setText
    );

    ui->labelCameraDetails->setText(ui->openGlArea->camera());
    QObject::connect(
        ui->openGlArea, &OpenGLArea::cameraChanged, ui->labelCameraDetails,
        &QLabel::setText
    );

    QObject::connect(
        ui->pushButtonAdd, &QPushButton::clicked, &m_addDialog, &QDialog::open
    );

    QObject::connect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );

    QObject::connect(
        ui->openGlArea, &OpenGLArea::objectClicked, this, &MainWindow::select
    );

    add(CursorObject);
    add(TorusObject);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::add(ObjectType objectType) {
    PVec4 position = {0.f, 0.f, 0.f, 1.f};
    for (uint i = 0; i < m_renderables.length(); i++) {
        if (m_renderables[i]->type() == ObjectType::CursorObject) {
            position = m_renderables[i]->model().position;
            break;
        }
    }

    IRenderable *renderable;
    switch (objectType) {
    case ObjectType::CursorObject:
        renderable = new Cursor();
        break;
    case ObjectType::PointObject:
        renderable = new Point(position);
        break;
    case ObjectType::TorusObject:
        renderable = new Torus(position);
        break;

    default:
        DPRINT("Unknown object type:" << objectType);
        return;
    }

    m_renderables.emplaceBack(renderable);

    ui->openGlArea->tryAddRenderable(renderable);
    ui->listWidget->addItem(renderable->listItem());
}

void MainWindow::removeSelected() {
    QObject::disconnect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );

    unbindParametersForSelected();
    for (uint idx : m_selected) {
        auto renderable = m_renderables[idx];

        ui->openGlArea->tryRemoveRenderable(renderable);
        ui->listWidget->removeItemWidget(renderable->listItem());

        m_renderables.removeAt(idx);
        delete renderable;
    }
    m_selected.clear();

    QObject::connect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );
}

void MainWindow::select(IRenderable *renderable) {
    QObject::disconnect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );

    for (uint idx : m_selected)
        m_renderables[idx]->listItem()->setSelected(false);
    if (renderable != nullptr)
        renderable->listItem()->setSelected(true);

    QObject::connect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );

    updateParametersUi();
}

void MainWindow::updateParametersUi() {
    unbindParametersForSelected();

    m_selected.clear();
    for (auto item : ui->listWidget->selectedItems()) {
        uint idx = m_renderables.count();
        while (--idx >= 0 && m_renderables[idx]->listItem() != item)
            ;
        if (idx == -1)
            continue;
        m_selected.append(idx);
    }

    bindParametersForSelected();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QObject::disconnect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );

    unbindParametersForSelected();
    m_selected.clear();

    for (auto renderable : m_renderables) {
        ui->openGlArea->tryRemoveRenderable(renderable);
        ui->listWidget->removeItemWidget(renderable->listItem());

        delete renderable;
    }
    m_renderables.clear();

    event->accept();
}

void MainWindow::bindParametersForSelected() {
    ui->tabObject->setEnabled(true);
    ui->openGlArea->setActive(nullptr);
    switch (m_selected.count()) {
    case 0: { // none are selected, nothing to add
        ui->tabObject->setEnabled(false);
        break;
    }
    case 1: {
        // one is selected, need to add its ui
        auto single = m_renderables[m_selected.first()];
        bindParametersForSingle(single);
        ui->openGlArea->setActive(single);
        break;
    }
    default: { // multiple are selected, need to add group ui
        QList<IRenderable *> group(m_selected.count());
        for (uint idx : m_selected)
            group.append(m_renderables[idx]);
        bindParametersForGroup(group);
        break;
    }
    }
}

void MainWindow::unbindParametersForSelected() {
    ui->tabObject->setEnabled(false);
    ui->openGlArea->setActive(nullptr);
    switch (m_selected.count()) {
    case 0: { // none are selected, nothing to add
        break;
    }
    case 1: {
        // one is selected, need to add its ui
        auto single = m_renderables[m_selected.first()];
        unbindParametersForSingle(single);
        break;
    }
    default: { // multiple are selected, need to add group ui
        QList<IRenderable *> group(m_selected.count());
        for (uint idx : m_selected)
            group.append(m_renderables[idx]);
        unbindParametersForGroup(group);
        break;
    }
    }
}

void MainWindow::bindParametersForSingle(IRenderable *renderable) {
    const auto widgets = renderable->ui();
    for (uint i = 0; i < widgets.size(); i++) {
        ui->verticalLayoutParameters->insertWidget(i, widgets[i]);
        widgets[i]->show();
    }

    ui->labelObjectName->setText(renderable->name());
    QObject::connect(
        renderable, &IRenderable::nameChanged, ui->labelObjectName,
        &QLabel::setText
    );
}

void MainWindow::unbindParametersForSingle(IRenderable *renderable) {
    for (auto widget : renderable->ui()) {
        ui->verticalLayoutParameters->removeWidget(widget);
        widget->hide();
    }

    QObject::disconnect(
        renderable, &IRenderable::nameChanged, ui->labelObjectName,
        &QLabel::setText
    );
    ui->labelObjectName->setText("Nothing selected");
}

void MainWindow::bindParametersForGroup(QList<IRenderable *> renderables) {
    // TODO
    ui->labelObjectName->setText(
        QString("Group of %1 objects").arg(QString::number(renderables.count()))
    );
}

void MainWindow::unbindParametersForGroup(QList<IRenderable *> renderables) {
    // TODO
    ui->labelObjectName->setText("Nothing selected");
}
