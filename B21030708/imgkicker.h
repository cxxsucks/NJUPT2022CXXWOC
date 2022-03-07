#pragma once
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QElapsedTimer>
class QEvent;
class QPropertyAnimation;

class ImgKicker : public QObject {
    Q_OBJECT;
    QPropertyAnimation* lastAnimation;
    QObject* lastReciever;
    QPoint lastStart;
    QElapsedTimer timePressed;

protected:
    bool eventFilter(QObject*, QEvent*) override;
public:
    ImgKicker(QObject* parent);
};
