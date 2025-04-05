#include "main_window.h"
#include "./ui_main_window.h"

#include "../cursor/cursor.h"
#include "../point/point.h"
#include "../polyline/polyline.h"
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
        ui->pushButtonConnect, &QPushButton::clicked, this,
        &MainWindow::connectSelected
    );

    QObject::connect(
        ui->pushButtonRemove, &QPushButton::clicked, this,
        &MainWindow::removeSelected
    );

    QObject::connect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );

    QObject::connect(
        ui->openGlArea, &OpenGLArea::objectClicked, this, &MainWindow::select
    );

    add(CursorObject);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::add(ObjectType objectType) {
    PVec4 position = {0.f, 0.f, 0.f, 1.f};
    for (uint i = 0; i < m_renderables.size(); i++) {
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

void MainWindow::connectSelected() {
    auto *polyline = new Polyline(m_selected);
    QObject::connect(
        this, &MainWindow::objectRemoved, polyline,
        &Polyline::tryRemoveControlPoint
    );
    QObject::connect(
        polyline, &Polyline::needRemoval, this, &MainWindow::removeObject
    );

    m_renderables.emplace_back(polyline);

    ui->openGlArea->tryAddRenderable(polyline);
    ui->listWidget->addItem(polyline->listItem());
}

void MainWindow::removeSelected() {
    QObject::disconnect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );

    unbindParametersForSelected();
    while (m_selected.size() != 0)
        removeObject(m_selected.last());

    QObject::connect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );
}

void MainWindow::removeObject(IRenderable *renderable) {
    if (renderable->type() == ObjectType::CursorObject)
        return;

    ui->openGlArea->tryRemoveRenderable(renderable);
    ui->listWidget->removeItemWidget(renderable->listItem());

    m_selected.removeAll(renderable);
    m_renderables.removeAll(renderable);

    emit objectRemoved(renderable);
    delete renderable;
}

void MainWindow::select(IRenderable *renderable) {
    QObject::disconnect(
        ui->listWidget, &QListWidget::itemSelectionChanged, this,
        &MainWindow::updateParametersUi
    );

    for (auto r : m_selected)
        r->listItem()->setSelected(false);
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
        uint idx = m_renderables.size();
        while (--idx >= 0 && m_renderables[idx]->listItem() != item)
            ;
        if (idx != -1)
            m_selected.append(m_renderables[idx]);
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
    ui->openGlArea->setActive({});
    switch (m_selected.size()) {
    case 0: { // none are selected, nothing to add
        ui->tabObject->setEnabled(false);
        break;
    }
    case 1: { // one is selected, need to add its ui
        auto single = m_selected.first();
        bindParametersForSingle(single);
        ui->openGlArea->setActive({single});
        break;
    }
    default: { // multiple are selected, need to add group ui
        bindParametersForGroup(m_selected);
        ui->openGlArea->setActive(m_selected);
        break;
    }
    }
}

void MainWindow::unbindParametersForSelected() {
    ui->tabObject->setEnabled(false);
    ui->openGlArea->setActive({});
    switch (m_selected.size()) {
    case 0: { // none are selected, nothing to remove
        break;
    }
    case 1: { // one is selected, need to remove its ui
        auto single = m_selected.first();
        unbindParametersForSingle(single);
        break;
    }
    default: { // multiple are selected, need to remove group ui
        unbindParametersForGroup(m_selected);
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
    ui->pushButtonConnect->setEnabled(true);
    ui->labelObjectName->setText(
        QString("Group of %1 objects").arg(QString::number(renderables.size()))
    );
}

void MainWindow::unbindParametersForGroup(QList<IRenderable *> renderables) {
    // TODO
    ui->pushButtonConnect->setEnabled(false);
    ui->labelObjectName->setText("Nothing selected");
}
