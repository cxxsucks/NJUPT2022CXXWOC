#include "imgscene.hpp"

#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QDebug>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    imgScene w;

    QString imgPath = QCoreApplication::applicationDirPath()
            + "/Images";
    // A directory from which images are searched is outputed.
    // Place your Images there 
    // Alternatively you can extend image selection functionality
    // with APIs provided by Qt and imgscene.hpp
    qDebug() << imgPath;
    
    w.addImageDirectory(":/fallback");
    if (w.addImageDirectory(imgPath) != 0) {
        w.eraseImageDirectory(":/fallback");
        // If your images are read, remove fallback(PvZ) ones
    }

    w.start();
    w.showMaximized();
    return a.exec();
}
