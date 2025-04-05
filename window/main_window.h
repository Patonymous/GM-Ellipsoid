#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include "../object_type.h"
#include "../renderable.h"
#include "add_object_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void add(ObjectType objectType);
    void removeSelected();

    void select(IRenderable *renderable);
    void updateParametersUi();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void bindParametersForSelected();
    void unbindParametersForSelected();

    void bindParametersForSingle(IRenderable *renderable);
    void unbindParametersForSingle(IRenderable *renderable);

    void bindParametersForGroup(QList<IRenderable *> renderables);
    void unbindParametersForGroup(QList<IRenderable *> renderables);

    Ui::MainWindow *ui;

    AddObjectDialog m_addDialog;

    QList<IRenderable *> m_renderables;
    QList<uint>          m_selected;
};

#endif // MAIN_WINDOW_H
