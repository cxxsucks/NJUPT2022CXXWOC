#include "imgscene.hpp"
#include <QtCore/QTimer>
#include <QtCore/QDir>
#include <QtGui/QBitmap>
#include <QtCore/QCoreApplication>
#include <QtGui/QShortcut>
#include <random>
#include <algorithm>
#include <QtWidgets/QFileDialog>
#include <QApplication>
#include <QScreen>

imgScene::imgScene(QWidget *parent, qsizetype _maxImg, int interval)
    : QWidget(parent), timer(this), maxImg(_maxImg) { 
    setInterval(interval);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(new QShortcut(QKeySequence(tr("Ctrl+0")), this),
        &QShortcut::activated,this,&imgScene::addImageDialog);


    connect(&timer, &QTimer::timeout, this, &imgScene::imgItemUpdate);
}

void imgScene::imgItemUpdate() {
    if (imgRecords.empty())
        return;
    /* Random Number Generation in C++17 way
     An Engine is created for number generation, whose result is
     piped to a Distribution
     Only the very basic ones is used here. Many more engines and
     Distributions are supported, like Normal distribution 
     If you hate their complexity, switch back to C-style is OK.*/
    static std::default_random_engine e;
    // Choose which image is to be added
    std::uniform_int_distribution<qsizetype> atRng(0, imgRecords.size() - 1);
    // Choose the rotation
    std::uniform_real_distribution<qreal> rotRng(0.0, 360.0);
    // Construct the image displayer at back
    // The operator() of distribution yields a random number 
    // that fits the distribution
    imgItems.emplace_back(this, &imgRecords[atRng(e)], rotRng(e));
    imgItem& added = imgItems.back();

    // Choose the location of the newly-added image
    std::uniform_int_distribution<int> 
        xRng(-added.width() / 2, width() - added.width() / 2),
        yRng(-added.height() / 2, height() - added.height() / 2);
    added.move(xRng(e), yRng(e));
    // Destroy the image when capacity exceeded(first in first out)
    while (imgItems.size() > maxImg)
        imgItems.pop_front();
    added.show();
}

void imgScene::setInterval(int interval) {
    if (interval > 0)
        timer.setInterval(interval);
}

bool imgScene::addImagePath(const QString &path) {
    if (QPixmap(path).isNull())
        return false;
    // Then destroy all displayers (for realloc)
    if (imgRecords.size() == imgRecords.capacity())
        resetScene();
    imgRecords.push_back(imgRecord(path));
    return true;
}

bool imgScene::eraseImagePath(const QString &path) {
    // Find the location of image at path
    QVector<imgRecord>::iterator iter = std::find_if(
        imgRecords.begin(), imgRecords.end(),
        [&](const imgRecord& r) { return r.getPath() == path; }
    );
    if (iter == imgRecords.cend())
        return false;  // Not found

    // Then destroy all displayers
    resetScene();
    // Finally, destroy the image data.
    imgRecords.erase(iter);
    return true;
}

qsizetype imgScene::addImageDirectory(const QString& path) {
    QDir dir = QDir::cleanPath(path);
    if (!dir.isReadable()) return 0;

    qsizetype res = 0;
    for (QFileInfo& fInfo : dir.entryInfoList(QDir::Files | QDir::Readable))
        res += static_cast<qsizetype>(addImagePath(fInfo.absoluteFilePath()));
    return res;
}

qsizetype imgScene::eraseImageDirectory(const QString& path) {
    QDir dir = QDir::cleanPath(path);
    if (!dir.isReadable()) return qsizetype();

    qsizetype res = 0;
    for (QFileInfo& fInfo : dir.entryInfoList(QDir::Files | QDir::Readable))
        res += static_cast<qsizetype>(eraseImagePath(fInfo.absoluteFilePath()));
    return res;
}

void imgScene::setMaxImgCount(size_t count) {
    if (count <= 0)
        return;
    /*The following erase overload takes two iterators 
    removing everything between.
    We need to remove the (size() - count) items that exceeds max size
    The n-th iterator is obtained by std::next */
    if (count < imgItems.size())
        imgItems.erase(imgItems.begin(),
            std::next(imgItems.begin(), imgItems.size() - count));
    maxImg = count;
}

void imgScene::clear() {
    // imgItems must be cleared first 
    // as existing imgItem rely on imgRecords
    imgItems.clear();
    imgRecords.clear();
}

void imgScene::start(int interval) {
    if (interval <= 0)
        timer.start();
    else timer.start(interval);
}
qsizetype imgScene::addImageDialog()
{
    QFileDialog dia(this, QStringLiteral("add image"), QCoreApplication::applicationDirPath());
    dia.setFileMode(QFileDialog::ExistingFiles);
    dia.exec();
    qsizetype res = 0;
    for (const QString& path : dia.selectedFiles())
        res += static_cast<qsizetype>(addImagePath(path));
    return res;
}
mw::mw(QWidget*parent) : QWidget(parent) {
    connect(new QShortcut(QKeySequence("Esc"), this),
        &QShortcut::activated, this, &mw::close);
    connect(new QShortcut(QKeySequence(tr("Ctrl+1")), this),
        &QShortcut::activated, this, &mw::showimage);
}
qsizetype mw::addimageDialog()
{
    QFileDialog dia(this, QStringLiteral("add image"), QCoreApplication::applicationDirPath());
    dia.setFileMode(QFileDialog::ExistingFile);
    dia.exec();
    return 0;
}
void mw::showimage()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenRect = screen->availableVirtualGeometry();
    mw::resize(screenRect.width(), screenRect.height());
    QPalette PAllbackground = mw::palette();
    QString fileName = QFileDialog::getOpenFileName(this, "open file", "", "");
    QImage ImgAllbackground = QImage(fileName);
    QImage fitimgpic = ImgAllbackground.scaled(mw::width(), mw::height()+40, Qt::IgnoreAspectRatio);
    PAllbackground.setBrush(QPalette::Window, QBrush(fitimgpic));
    mw::setPalette(PAllbackground);
    mw::showFullScreen();

}
