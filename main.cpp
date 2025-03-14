#include "ellipsoid/ellipsoid_window.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication    a(argc, argv);
    EllipsoidWindow w;
    w.show();
    return a.exec();
}
