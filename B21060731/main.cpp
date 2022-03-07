#include "imgscene.hpp"
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include<QtMultimedia/QMediaPlayer>
#include<QtMultimedia/QAudioOutput>
#include<QtWidgets/QLayout>
#include <QtGui/QShortcut>
#include <QtGui/QScreen>
#include <QtWidgets/QFileDialog>
#include<QtWidgets/qlabel.h>


int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    mw* mainw = new mw;
    mainw->show();
    QLabel* l = new QLabel;
    l->setText("先选择音乐。按ctrl+0（零）可以选择图片，ctrl+1可以选择背景图片，ese可以退出程序");
    l->show();

    QMediaPlayer* player = new QMediaPlayer;
    QAudioOutput* audiooutput = new QAudioOutput;
    player->setAudioOutput(audiooutput);
    QString fileName = QFileDialog::getOpenFileName(l, "open file", "", "");
    player->setSource(QUrl::fromLocalFile(fileName));
    player->play();

    QString imgPath = QCoreApplication::applicationDirPath()
        + "/Images";
    // A directory from which images are searched is outputed.
    // Place your Images there 
    // Alternatively you can extend image selection functionality
    // with APIs provided by Qt and imgscene.hpp
    qDebug() << imgPath;

    QVBoxLayout* lay = new QVBoxLayout(mainw);
    imgScene* w = new imgScene(mainw);
    lay->addWidget(w);
    lay->addWidget(l);
    lay->setStretch(0, 1);
//    w.show();
//    w.setGeometry(QRect(QPoint(0, 0), mainw.size()));
    w->start();
    int res = a.exec();
    delete mainw;
    return res;
}
