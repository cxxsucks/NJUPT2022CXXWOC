#include "preload.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    imgScene* w = preload();
    return a.exec();
}
