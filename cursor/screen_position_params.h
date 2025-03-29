#ifndef SCREEN_POSITION_PARAMS_H
#define SCREEN_POSITION_PARAMS_H

#include <QWidget>

#include "screen_position_dialog.h"

class Cursor;

QT_BEGIN_NAMESPACE
namespace Ui {
    class ScreenPositionParams;
} // namespace Ui
QT_END_NAMESPACE

class ScreenPositionParams : public QWidget {
    Q_OBJECT

public:
    ScreenPositionParams(QWidget *parent = nullptr);
    ~ScreenPositionParams();

    void setupConnections(Cursor *cursor);

private slots:
    void updateScreenPositionLabels(float x, float y);

private:
    Ui::ScreenPositionParams *ui;

    ScreenPositionDialog m_dialog;
};

#endif // SCREEN_POSITION_PARAMS_H
