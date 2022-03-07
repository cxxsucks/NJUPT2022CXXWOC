#include "imgitem.h"

#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <random>

int imgRecord::refCount = 0;

imgRecord::imgRecord(const imgRecord& origin)
    : path(origin.path), myPix(origin.myPix) {
    refCount++;
}

imgRecord::imgRecord(const QString& _path)
    :path(_path), myPix(nullptr) {
}

imgRecord::imgRecord(imgRecord&& obsolete)
    : path(obsolete.path), myPix(obsolete.myPix) {
    refCount++;
    obsolete.myPix = nullptr;
}

void imgRecord::clear() {
    if (--refCount)
        delete myPix;
}

void imgItem::paintEvent(QPaintEvent *e) {
    QPixmap* toDraw = pix.get();
    QPainter painter(this);
    painter.setTransform(matrix1 * matrix2 * matrix3 * matrix4 * matrix5);
    painter.drawPixmap(0, 0, *toDraw);
}

void imgItem::mousePressEvent(QMouseEvent *e) {
    emit clicked(clickEffect);
    e->accept();
}

imgItem::imgItem(QWidget *parent, imgRecord& _pix)
    : QWidget(parent), pix(_pix), length(-1), timer(nullptr) {
    connect(this, &imgItem::clicked, this, &imgItem::onClicked);
}

void imgItem::setTime(int _length = 0, int _wait = 0, int _per = 35) {
    length = _length;
    wait = _wait;
    per = _per;
}

void imgItem::start() {
    if (length == -1)
        setTime();
    if (wait) {
        timer = new QTimer(this);
        timer->singleShot(wait, [this]() { emit begin(); update(); });
    } else {
        emit begin(); 
        update();
    }
    if (length) {
        QTimer* timer = new QTimer(this);
        timer->singleShot(length, [this]() { emit end(); hide(); });
    }
}

void imgItem::setScaling(double x_time = 0, double y_time = 0, int wait = 0) {
    if (!x_time && !y_time) {
        std::default_random_engine e;
        std::uniform_real_distribution<double> time(0.5, 2.0);
        x_time = y_time = time(e);
    }
    if (wait) {
        QTimer* timer = new QTimer(this);
        timer->singleShot(wait, [this, x_time, y_time]() {
            matrix1.scale(x_time, y_time);
            update();
        });
        return;
    }
    matrix1.scale(x_time, y_time);
}

void imgItem::setShearing(double deg_x, double deg_y, int wait = 0) {
    if (wait) {
        QTimer* timer = new QTimer(this);
        timer->singleShot(wait, [this, deg_x, deg_y]() {
            matrix2.shear(deg_x, deg_y);
            update();
        });
        return;
    }
    matrix2.shear(deg_x, deg_y);
}

void imgItem::setRotation(double deg = 0, int wait = 0) {
    QPixmap* pic = pix.get();
    if (!deg) {
        std::default_random_engine e;
        std::uniform_real_distribution<double> rotRng(0.0, 360.0);
        deg = rotRng(e);
    }
    if (wait) {
        QTimer* timer = new QTimer(this);
        timer->singleShot(wait, [this, pic, deg]() {
            matrix3.translate(pic->width() / 2, pic->height() / 2);
            matrix4.rotate(deg);
            matrix5.translate(-pic->width() / 2, -pic->height() / 2); 
            update();
        });
        return;
    }
    matrix3.translate(pic->width() / 2, pic->height() / 2);
    matrix4.rotate(deg);
    matrix5.translate(-pic->width() / 2, -pic->height() / 2);
}

void imgItem::setTranslation(double x, double y, int wait = 0) {
    if (wait) {
        QTimer* timer = new QTimer(this);
        timer->singleShot(wait, [this, x, y]() {
            matrix5 *= QTransform::fromTranslate(x, y);
            update();
        });
        return;
    }
    matrix5 *= QTransform::fromTranslate(x, y);
}

void imgItem::setSmoothScaling(double x_time = 0, double y_time = 0, int wait = 0, int length = 0) {
    if (!x_time && !y_time) {
        std::default_random_engine e;
        std::uniform_real_distribution<double> time(0.5, 2.0);
        x_time = y_time = time(e);
    }
    if (!length)
        length = this->length;
    if (!wait)
        wait = per;
    connect(this, &imgItem::begin, [this, x_time, y_time, length, wait]() {
        QTimer* timer = new QTimer(this);
        timer->singleShot(wait, [this, x_time, y_time, length, wait]() {
            double dx = x_time * this->per / (length - wait);
            double dy = y_time * this->per / (length - wait);
            connect(this->timer, &QTimer::timeout, [this, dx, dy]() {
                static double x_time = 0, y_time = 0;
                x_time += dx;
                y_time += dy;
                matrix1.scale(x_time, y_time);
                });
            });
        });
}

void imgItem::setSmoothShearing(double deg_x, double deg_y, int wait = 0, int length = 0) {
    if (!length)
        length = this->length;
    if (!wait)
        wait = per;
    connect(this, &imgItem::begin, [this, deg_x, deg_y, length, wait]() {
        QTimer* timer = new QTimer(this);
        timer->singleShot(wait, [this, deg_x, deg_y, length, wait]() {
            double dx = deg_x * this->per / (length - wait);
            double dy = deg_y * this->per / (length - wait);
            connect(this->timer, &QTimer::timeout, [this, dx, dy]() {
                static double deg_x = 0, deg_y = 0;
                deg_x += dx;
                deg_y += dy;
                matrix2.shear(deg_x, deg_y);
                });
            });
    });
}

void imgItem::setSmoothRotation(double deg = 0, int wait = 0, double speed = 50) {
    QPixmap* pic = pix.get();
    if (!deg) {
        std::default_random_engine e;
        std::uniform_int_distribution<int> circle(1, 10);
        deg = circle(e) * 360;
    }
    if (!wait)
        wait = per;
    connect(this, &imgItem::begin, [this, pic, deg, speed, wait]() {
        QTimer* timer = new QTimer(this);
        timer->singleShot(wait, [this, pic, deg, speed]() {
            int w = pic->width(), h = pic->height();
            connect(this->timer, &QTimer::timeout, [this, deg, w, h, speed]() {
                static double amount = 0;
                amount += speed / 1000 < deg - amount ? speed / 1000 : (deg - amount);
                matrix3.translate(w / 2.0, h / 2.0);
                matrix4.rotate(amount);
                matrix5.translate(-w / 2.0, -h / 2.0);
                });
            });
        });
}

void imgItem::setSmoothTranslation(double x, double y, int wait = 0, double speed = 100) {
    if (!length)
        length = this->length;
    if (!wait)
        wait = per;
    connect(this, &imgItem::begin, [this, x, y, speed, wait]() {
        QTimer* timer = new QTimer(this);
        timer->singleShot(wait, [this, x, y, speed, wait]() {
            const double dx = x / (x * x + y * y) * speed / 1000;
            const double dy = y / (x * x + y * y) * speed / 1000;
            connect(this->timer, &QTimer::timeout, [this, dx, dy]() {
                matrix5 *= QTransform::fromTranslate(dx, dy);
            });
        });
    });
}

void imgItem::onClicked(std::function<void(imgItem*)> fuc) {
    fuc(this);
}
