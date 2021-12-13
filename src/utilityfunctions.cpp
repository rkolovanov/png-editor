#include <utilityfunctions.h>


QString pointToString(const QPoint& point) {
    return "[X: " + QString::number(point.x()) + ", Y: " + QString::number(point.y()) + "]";
}

QString colorToString(const QColor& color) {
    return "[R: " + QString::number(color.red()) + ", G: " + QString::number(color.green()) + ", B: " + QString::number(color.blue())+ ", A: " + QString::number(color.alpha()) + "]";
}

QColor getPixel(png_bytep pixel) {
    if (pixel != nullptr) {
        return QColor(pixel[0], pixel[1], pixel[2], pixel[3]);
    }

    return QColor();
}

void setPixel(png_bytep pixel, const QColor& color) {
    if (pixel != nullptr) {
        pixel[0] = color.red();
        pixel[1] = color.green();
        pixel[2] = color.blue();
        pixel[3] = color.alpha();
    }
}

QColor getColorFromPixel(png_bytep pixel) {
    return QColor(pixel[0], pixel[1], pixel[2], pixel[3]);
}

void invertPixelColor(png_bytep pixel) {
    if (pixel != nullptr) {
        pixel[0] = 255 - pixel[0];
        pixel[1] = 255 - pixel[1];
        pixel[2] = 255 - pixel[2];
    }
}

QToolButton* createToolButtonColor(QWidget* parent, QString objectName, QString toolTip, const QColor& color, bool checkable) {
    QPixmap pixmap(39, 16);
    QToolButton* button = new QToolButton(parent);

    pixmap.fill(color);
    button->setObjectName(objectName);
    button->setToolTip(toolTip);
    button->setIcon(QIcon(pixmap));

    if (checkable) {
        button->setCheckable(checkable);
    }

    return button;
}

QToolButton* createToolButton(QWidget* parent, QString objectName, QString toolTip, QString path, bool checkable) {
    QToolButton* button = new QToolButton(parent);

    button->setObjectName(objectName);
    button->setToolTip(toolTip);
    button->setIcon(QIcon(path));

    if (checkable) {
        button->setCheckable(checkable);
    }

    return button;
}

QSpinBox* createSpinBox(QWidget* parent, int initialValue, int minValue, int maxValue, QString toolTip) {
    QSpinBox* spinBox = new QSpinBox(parent);

    spinBox->setValue(initialValue);
    spinBox->setRange(minValue, maxValue);
    spinBox->setToolTip(toolTip);

    return spinBox;
}
