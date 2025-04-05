#ifndef ADD_OBJECT_DIALOG_H
#define ADD_OBJECT_DIALOG_H

#include <QDialog>

class MainWindow;

QT_BEGIN_NAMESPACE
namespace Ui {
    class AddObjectDialog;
} // namespace Ui
QT_END_NAMESPACE

class AddObjectDialog : public QDialog {
    Q_OBJECT

public:
    AddObjectDialog(QWidget *parent = nullptr);
    ~AddObjectDialog();

    void setupConnections(MainWindow *cursor);

private slots:
    void requestAddTorus();
    void requestAddPoint();

private:
    Ui::AddObjectDialog *ui;

    MainWindow *m_window;
};

#endif // ADD_OBJECT_DIALOG_H
