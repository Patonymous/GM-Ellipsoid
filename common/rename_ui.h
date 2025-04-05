#ifndef RENAME_UI_H
#define RENAME_UI_H

#include <QWidget>

class IRenderable;

QT_BEGIN_NAMESPACE
namespace Ui {
    class RenameUi;
}
QT_END_NAMESPACE

class RenameUi : public QWidget {
    Q_OBJECT

public:
    RenameUi(QWidget *parent = nullptr);
    ~RenameUi();

    void setupConnections(IRenderable *renderable);

private:
    void updateName();

    Ui::RenameUi *ui;

    IRenderable *m_renderable;
};

#endif // RENAME_UI_H
