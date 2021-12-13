#include "imageview.h"
#include <QDebug>
#include <QMouseEvent>


ImageView::ImageView(QWidget *parent) : QGraphicsView(parent) {
    setMouseTracking(true);
    setObjectName("imageView");
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    scene = new ImageScene(0, 0, 0, 0, this);
    scene->setObjectName("imageScene");

    setScene(scene);
}

void ImageView::setImage(ImagePNG* image) {
    QPixmap imagePixmap = image->getPixmap();

    clearScene(image->getWidth(), image->getHeight());
    scene->addPixmap(imagePixmap);

    setScene(scene);
}

void ImageView::clearScene(int width, int height) {
    scene->clear();
    scene->setSceneRect(0, 0, width, height);
    setScene(scene);
}

ImageScene::ImageScene(qreal x, qreal y, qreal width, qreal height, QObject *parent):
    QGraphicsScene(x, y, width, height, parent) {}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    emit mousePositionChanged(event);
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    emit point1Selected(event);
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    emit point2Selected(event);
}
