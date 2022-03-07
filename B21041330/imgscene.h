#ifndef IMGSCENE_H
#define IMGSCENE_H

#include "imgitem.h"

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <list>
#include <vector>

// Forward Declarations for classes only used in header
class QPixmap;

/*! @class imgScene
 @brief A widget displaying images scattering randomly inside it. 
 Used @b imgRecord to reduce memory usage
 @note @b std::list is used because @c QLinkedList is obsolete. */
class imgScene : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(imgScene)
    QTimer* timer;
    std::size_t maxImg;
    std::function<void(item*)> clickEffect = [this](item* target) { resetScene(); };
    // A linked list of imgItems for queue simulation 
    // and optimized erasure
    std::list<imgItem> imgItems;
    // Image data are not as frequently moved and erased on the other hand.
    // They DO need to be randomly accessed on ImgItem creation.
    std::vector<imgRecord> imgRecords;

    //! @brief Do something to items when timer timeout.
    void imgItemUpdate();
public:
    imgScene(QWidget *parent = nullptr);
    //! @warning The call to @c clear is necessary. Try figure out why.
    ~imgScene() {clear();}

    /*! @brief Add a single image file at path
     @return `true` on success, `false` otherwise */
    bool addImage(const QString& path);
    void setMaxImgCount(size_t count);
    //! @brief Clear the images displayers AND image data
    void clear();
    void setInterval(int interval);

signals:
    void clicked(std::function<void(item*)>);
    void begin();
    void end();    

protected:
    void mousePressEvent(QMouseEvent* e) override;

public slots:
    void start(int length);
    //! @brief Pauses image displaying.
    void stop() {timer->stop();}
    //! @brief Clear the image displayers. Image data remain unchanged.
    void resetScene() {imgItems.clear();}
    void start();
    void onClicked(std::function<void(item*)>);
};

#endif // IMGSCENE_H
