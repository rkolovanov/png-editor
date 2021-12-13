#ifndef UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

#include "png.h"
#include <QString>
#include <QPoint>
#include <QColor>
#include <QToolButton>
#include <QSpinBox>


QString pointToString(const QPoint& point);
QString colorToString(const QColor& color);
QColor getPixel(png_bytep pixel);
void setPixel(png_bytep pixel, const QColor& color);
QColor getColorFromPixel(png_bytep pixel);
void invertPixelColor(png_bytep pixel);
QToolButton* createToolButtonColor(QWidget* parent, QString objectName, QString toolTip, const QColor& color, bool checkable);
QToolButton* createToolButton(QWidget* parent, QString objectName, QString toolTip, QString path, bool checkable);
QSpinBox* createSpinBox(QWidget* parent, int initialValue, int minValue, int maxValue, QString toolTip);


#endif // UTILITY_FUNCTIONS_H
