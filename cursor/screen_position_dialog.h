#ifndef SCREEN_POSITION_DIALOG_H
#define SCREEN_POSITION_DIALOG_H

#include <QDialog>

class Cursor;

QT_BEGIN_NAMESPACE
namespace Ui {
    class ScreenPositionDialog;
} // namespace Ui
QT_END_NAMESPACE

class ScreenPositionDialog : public QDialog {
    Q_OBJECT

public:
    ScreenPositionDialog(QWidget *parent = nullptr);
    ~ScreenPositionDialog();

    void setupConnections(Cursor *cursor);

private slots:
    void requestScreenPosition();

private:
    Ui::ScreenPositionDialog *ui;

    Cursor *m_cursor;
};

#endif // SCREEN_POSITION_DIALOG_H
