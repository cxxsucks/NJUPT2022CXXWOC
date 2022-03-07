#include "imgscene.hpp"
#include <QtWidgets/QApplication>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFrame>
#include <QWidget>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    imgScene w;

    w.addImageDirectory(":/fallback");
    if (w.addImageDialog() != 0) {
        w.eraseImageDirectory(":/fallback");
        // If your images are read, remove fallback(PvZ) ones
    }

    QString imgPath=QCoreApplication::applicationDirPath();
    qDebug()<<imgPath;

    //add music
    QMediaPlayer*player = new QMediaPlayer;
    QAudioOutput*audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile(
            QCoreApplication::applicationDirPath()+"/music.mp3"
                         ));
    player->play();
    player->setLoops(-1);//loop the music

    w.start();
    w.showMaximized();
    return a.exec();
}
