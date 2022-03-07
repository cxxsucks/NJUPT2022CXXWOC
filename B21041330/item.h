#ifndef ITEM_H
#define ITEM_H

#include <QtWidgets/QWidget>

class imgScene;

class item : public QWidget {
    Q_OBJECT
protected:
    static imgScene* scene;
signals:
    virtual void begin();
    virtual void end();
public slots:
    virtual void start();
};

#endif