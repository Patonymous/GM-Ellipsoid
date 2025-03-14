#include "torus/torus_window.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    TorusWindow  w;
    w.show();
    return a.exec();
}
