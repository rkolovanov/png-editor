#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <imagepng.h>


class ImageScene: public QGraphicsScene {
    Q_OBJECT

public:
    ImageScene(qreal x, qreal y, qreal width, qreal height, QObject* parent = nullptr);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

signals:
    void point1Selected(QGraphicsSceneMouseEvent* event);
    void point2Selected(QGraphicsSceneMouseEvent* event);
    void mousePositionChanged(QGraphicsSceneMouseEvent* event);
};


class ImageView: public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageView(QWidget* parent = nullptr);
    void setImage(ImagePNG* image);
    void clearScene(int width = 0, int height = 0);

private:
    ImageScene* scene = nullptr;
};


#endif // IMAGEVIEW_H
