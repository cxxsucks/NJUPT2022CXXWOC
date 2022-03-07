#include "imgscene.hpp"
#include <QtMultimedia/QSoundEffect>
#include <QtCore/QDebug>
#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QtMultimediaWidgets>
#include <QtGui/QKeyEvent>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    imgScene w;

    QString rescPath = QCoreApplication::applicationDirPath()
            + "/Resc";
    // A directory from which images are searched is outputed.
    // Place your Images there 
    // Alternatively you can extend image selection functionality
    // with APIs provided by Qt and imgscene.hpp
    qDebug() << rescPath;
    
    w.addImageDirectory(":/fallback");
    if (w.addImageDirectory(rescPath) != 0) {
        w.eraseImageDirectory(":/fallback");
        // If your images are read, remove fallback(PvZ) ones
    }

    QMediaPlayer *player = new QMediaPlayer;
    QAudioOutput *audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(rescPath + "/1.mp3"));
    audioOutput->setVolume(50);
    player->play();

    QWidget *widget = new QWidget(&w);
    widget->setGeometry(w.geometry());
    widget->setAutoFillBackground(true);
    QPalette palette;
    QPixmap pixmap(rescPath + "/ranran.png");
    palette.setBrush(QPalette::Window, QBrush(pixmap));
    widget->setPalette(palette);
    widget->resize(2000,1000);
    widget->show();

    w.start();
    w.showMaximized();
    return a.exec();
}
