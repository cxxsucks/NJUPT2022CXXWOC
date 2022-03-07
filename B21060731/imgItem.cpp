#include "imgscene.hpp"
#include <cmath>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QtEvents>
#include <QtCore/QDebug>

qsizetype imgRecord::addCounter(qptrdiff time) {
    qptrdiff cur = used + time;
    if (cur < 0) {
        qDebug() << "Negative Counter for:" << path;
        return 0;
    }

    // Usage Conter became non-zero. Data is in used and must be created.
    if (cur > 0 && used <= 0)
        myPix = new QPixmap(path);
    // Usage Counter dropped to zero. Data is not in use and can be deleted.
    else if (cur <= 0 && used > 0)
        { delete myPix; myPix = nullptr; }
    used = cur;
    return used;
}

imgRecord::imgRecord(const QString &path) {
    this->path = path;
    used = 0;
    myPix = nullptr;
}

imgRecord::imgRecord(imgRecord&& r) noexcept
    : path(std::move(r.path)), myPix(r.myPix), used(r.used) {
    r.myPix = nullptr;
    r.used = 0;
}

imgRecord& imgRecord::operator=(imgRecord&& r) noexcept {
    if (this != &r) {
        this->~imgRecord();
        new (this) imgRecord(std::forward<imgRecord>(r));
    }
    return *this;
}

imgRecord::imgRecord(const imgRecord& r) 
    : path(r.path), myPix(nullptr), used(0) {}

imgRecord& imgRecord::operator=(const imgRecord& r) {
    if (this != &r) {
        this->~imgRecord();
        new (this) imgRecord(r);
    }
    return *this;
}

imgRecord::~imgRecord() noexcept {
    // Normally all users must be killed before tracker gets killed 
    // or operations on users may visit null pointer
    if (used > 0) {
    // Warn if not.
        qDebug() << "Warning: Image" << path
             << "Destroyed on non-zero usage";
        delete myPix;
    }
}
//! @global C math functions use radian whereas Qt uses degree
static constexpr double radPerDeg = 0.01745329252;
void imgItem::setImgRecord(imgRecord *pixNew) {
    if (pixNew == pix) return; // Nothing to do :)
    if (pix)  // One less use for old image
        pix->addCounter(-1);
    if (pixNew) { // One more usage for new image
        pixNew->addCounter(1);
        const int h = pixNew->get()->height(),
            w = pixNew->get()->width();
        const double rotateRad = rotateDeg * radPerDeg;
        // To avoid namespace collision, add `::` before C library call    
        resize(1.5 * (w * ::abs(::sin(rotateRad)) + h * ::abs(::cos(rotateRad))),
               1.5 * (h * ::abs(::sin(rotateRad)) + w * ::abs(::cos(rotateRad))));
    } 
    pix = pixNew;
}

void imgItem::setRotationDegree(double deg) {
    rotateDeg = ::fmod(deg, 360.0);
    imgRecord* old = getImgRecord();
    if (!old) return;

    const int h = old->get()->height(),
        w = old->get()->width();
    const double rotateRad = rotateDeg * radPerDeg;

    resize(1.5 * (w * ::abs(::sin(rotateRad)) + h * ::abs(::cos(rotateRad))),
            1.5 * (h * ::abs(::sin(rotateRad)) + w * ::abs(::cos(rotateRad))));
}

void imgItem::paintEvent(QPaintEvent *e) {
    QPixmap* toDraw;
    if (!pix || !(toDraw = pix->get()))
        return QWidget::paintEvent(e);
    QPainter painter(this);
    painter.save();
    // Move to center, rotate, move to starting point, paint
    painter.translate(width() / 2, height() / 2);
    painter.rotate(rotateDeg);

    painter.drawPixmap(-toDraw->width() / 2, -toDraw->height() / 2, *toDraw);
    painter.restore();
    QWidget::paintEvent(e);
}

void imgItem::mousePressEvent(QMouseEvent *e) {
    emit clicked();
    e->accept();
}

imgItem::imgItem(QWidget *parent, imgRecord *_pix, double rot)
    : QWidget(parent), pix(nullptr), rotateDeg(0.0) {
    setRotationDegree(rot);
    setImgRecord(_pix);
}

imgItem::~imgItem() {
    setImgRecord(nullptr);
}
