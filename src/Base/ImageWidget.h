/**
   @author Shin'ichiro Nakaoka
*/

#ifndef CNOID_BASE_IMAGE_WIDGET_H
#define CNOID_BASE_IMAGE_WIDGET_H

#include <QWidget>
#include <cnoid/Image>
#include "exportdecl.h"

namespace cnoid {

class Image;

class CNOID_EXPORT ImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget* parent = 0);

//public Q_SLOTS:
    void setPixmap(const QPixmap& pixmap);
    void setImage(const Image& image);
    void setImage(const QImage& image);

    void setScalingEnabled(bool on);
    bool isScalingEnabled() const;

    void zoom(double scale);
    void translate(QPoint pos);

    bool getTransform(QTransform& transform);
    double getAngle();
    void setTransform(const QTransform& transform);
    void setAngle(double angle);
    void rotate(double angle);
    Image& getImage();

  protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual QSize sizeHint() const;
    virtual void resizeEvent(QResizeEvent *event);
        
private:
    QPixmap pixmap_;
    bool isScalingEnabled_;
    QTransform transform_;
    QTransform notScaledTransform_;
    Image transformedImage;

    void fitCenter();
    void resize(const QSize& size);
    bool fitted;
    QSize oldSize;
    double oldScale;
    bool settedT;

};

}

#endif
