#include "imgscene.h"

#include <QMouseEvent>
#include <QTimer>
#include <random>
#include <algorithm>

imgScene::imgScene(QWidget *parent)
    : QWidget(parent), timer(new QTimer(this)) {
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(timer, &QTimer::timeout, this, &imgScene::imgItemUpdate);
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
    // Construct the image displayer at back
    // The operator() of distribution yields a random number 
    // that fits the distribution
    imgItems.emplace_back(this, imgRecords[atRng(e)]);
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
        timer->setInterval(interval);
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

void imgScene::mousePressEvent(QMouseEvent* e) {
    emit clicked(clickEffect);
    e->accept();
}