#ifndef IMGITEM_H
#define IMGITEM_H

#include "item.h"

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QTransform>
#include <QtWidgets/QWidget>

class imgScene;

class imgRecord {
    const QString& path;
    QPixmap* myPix;
    static int refCount;
public:
    imgRecord(const QString&);
    imgRecord(imgRecord&&);
    imgRecord(const imgRecord&);

    QPixmap* get() { if (!myPix) load(); return myPix; }
    bool load();
    void clear();
};

/*! @class imgItem
 @brief A widget showing an image rotated to an angle.
 Used @b imgRecord to reduce memory usage*/
class imgItem : public QWidget {
    Q_OBJECT
private:
    imgRecord& pix;
    QTransform matrix1, matrix2, matrix3, matrix4, matrix5;
    int length, wait, per;
    QTimer* timer;
    std::function<void(imgItem*)> clickEffect = [](imgItem* img) { img->~imgItem(); };

public:
    imgItem(QWidget* parent, imgRecord& _pix);
    ~imgItem() { pix.clear(); };

    void setRotation(double deg, int wait);
    void setScaling(double x_time, double y_time, int wait);
    void setShearing(double deg_x, double deg_y, int wait);
    void setTranslation(double x, double y, int wait);
    void setSmoothRotation(double deg, int wait, double speed);
    void setSmoothScaling(double x_time, double y_time, int length, int wait);
    void setSmoothShearing(double deg_x, double deg_y,int length, int wait);
    void setSmoothTranslation(double x, double y, int wait, double speed);
    void setTime(int length, int wait, int per);

signals:
    void begin();
    void end();
    void clicked(std::function<void(imgItem*)>);

public slots:
    void start();
    void onClicked(std::function<void(imgItem*)>);

protected:
    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
};

#endif
