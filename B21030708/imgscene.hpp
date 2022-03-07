#ifndef IMGSCENE_H
#define IMGSCENE_H
#include <QtWidgets/QWidget>
#include <list>
#include <QtCore/QTimer>

// Forward Declarations for classes only used in header
class QPixmap;
class imgScene;
/*! @class imgRecord
 @brief A memory savvy QPixmap manager which 
 allocate the pixmap from @b path only if it is being used
 @note Advanced challengers can try to generalize and templatify the class
 @code 
 template<class T> class track_ptr
 // The instantiation used in this program is:
 tracker_ptr<QPixmap> pix(path, [](QString a){return new QPixmap(a);})
 @endcode */
class imgRecord {
    QString path;
    QPixmap* myPix;
    qsizetype used;
public:
    imgRecord(const QString& path);
    imgRecord(imgRecord&& r) noexcept;
    imgRecord& operator=(imgRecord&& r) noexcept;
    imgRecord(const imgRecord& r);
    imgRecord& operator=(const imgRecord& r);
    ~imgRecord() noexcept;

    /*! @return Pointer to data
    @retval `nullptr` if usage counter is at zero. */
    QPixmap* get() noexcept {return myPix;}
    const QString& getPath() const noexcept {return path;}
    /*! @brief Notify the class that the counter changed.
     If counter drops to zero, the data is freed.
     @return The counter after change
     @param time The number (signed) added to counter. */
    qsizetype addCounter(qptrdiff time);
    // The following implementation also works:
    // return const_cast<imgRecord*>(this)->addCounter(0);
    qsizetype getCounter() const noexcept { return used; }
};
/*! @class imgItem
 @brief A widget showing an image rotated to an angle. 
 Used @b imgRecord to reduce memory usage*/
class imgItem : public QWidget {
    Q_OBJECT
    imgRecord* pix;
    qreal rotateDeg;
    Q_DISABLE_COPY_MOVE(imgItem)
public:
    imgItem(QWidget* parent, imgRecord* _pix, double rot);
    ~imgItem();

    void setImgRecord(imgRecord* _pix);
    imgRecord* getImgRecord() const noexcept {return pix;}
    void setRotationDegree(double deg);
signals:
    void clicked();
protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent* e) override;
};

/*! @class imgScene
 @brief A widget displaying images scattering randomly inside it. 
 Used @b imgRecord to reduce memory usage
 @note @b std::list is used because @c QLinkedList is obsolete. */
class imgScene : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(imgScene)
    QTimer timer;
    std::size_t maxImg;

    // A linked list of imgItems for queue simulation 
    // and optimized erasure
    std::list<imgItem> imgItems;
    // Image data are not as frequently moved and erased on the other hand.
    // They DO need to be randomly accessed on ImgItem creation.
    QVector<imgRecord> imgRecords;

    //! @brief Do something to items when timer timeout.
    void imgItemUpdate();
public:
    imgScene(QWidget *parent = nullptr, qsizetype _maxImg = 10,
             int interval = 300);
    //! @warning The call to @c clear is necessary. Try figure out why.
    virtual ~imgScene() {clear();}

    /*! @brief Add a single image file at path
     @return `true` on success, `false` otherwise */
    bool addImagePath(const QString& path);
    /*! @brief Remove a single image file at path
     @return `true` on success, `false` otherwise */
    bool eraseImagePath(const QString& path);
    /*! @brief Add all images stored at path non-recursively
     @return Valid images added */
    qsizetype addImageDirectory(const QString& path);
    /*! @brief Remove all images stored at path non-recursively
     @return Images erased */
    qsizetype eraseImageDirectory(const QString& path);
    /*! @brief Set maximum images displayed at the widget
     @note If the images on scene already exceeded the limit,
     those created earlist would be instantly destroyed. */
    void setMaxImgCount(size_t count);
    //! @brief Clear the images displayers AND image data
    void clear();
    void setInterval(int interval);
protected:
     void paintEvent(QPaintEvent *e) override;
     void closeEvent(QCloseEvent *event)override;
public slots:
    /*! @brief Starts image displaying.
     @param interval The interval between updates.
     Remain unchanged when illegal. Default -1. */
    void start(int interval = -1);
    //! @brief Pauses image displaying.
    void stop() {timer.stop();}
    //! @brief Clear the image displayers. Image data remain unchanged.
    void resetScene() {imgItems.clear();}
    qsizetype addImageDialog();
};
#endif // IMGSCENE_H
