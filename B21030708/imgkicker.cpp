#include "imgkicker.h"
#include <QtCore/QPropertyAnimation>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QWidget>

bool ImgKicker::eventFilter(QObject* rec, QEvent* ev) {
    if (ev->type() == QEvent::MouseButtonPress) {
        delete lastAnimation;
        lastReciever = rec;
        lastStart = static_cast<QMouseEvent*>(ev)->globalPos();
        timePressed.start();
        return true;
    }
    else if (ev->type() == QEvent::MouseButtonRelease) {
        QWidget* wRec = qobject_cast<QWidget*>(rec);
        if (!wRec || rec != lastReciever)
            return QObject::eventFilter(rec, ev);
        const int dX = static_cast<QMouseEvent*>(ev)->globalPos().x() - lastStart.x(),
            dY = static_cast<QMouseEvent*>(ev)->globalPos().y() - lastStart.y();

        lastAnimation = new QPropertyAnimation(rec, "geometry");
        lastAnimation->setDuration(4 * timePressed.elapsed());
        lastAnimation->setStartValue(wRec->geometry());
        lastAnimation->setEndValue(wRec->geometry().adjusted(
            dX * 4, dY * 4, dX * 4, dY * 4
        ));
        lastAnimation->start();
        return true;
    }
    return QObject::eventFilter(rec, ev);
}

ImgKicker::ImgKicker(QObject* parent) : QObject(parent) {
    lastAnimation = nullptr;
    lastReciever = nullptr;
}
