#ifndef IMAGEPNG_H
#define IMAGEPNG_H

#include <png.h>
#include <QPixmap>
#include <QFileInfo>
#include "enumerations.h"


class ImagePNG {
private:
    struct ImagePart;

    int width = 0;
    int height = 0;
    png_byte colorType = 0;
    png_byte bitDepth = 0;
    png_byte interlaceType = 0;
    png_byte filteringMethod = 0;
    png_byte compressionType = 0;
    png_bytepp pixelArray = nullptr;

    QFileInfo fileInfo;
    bool loaded = false;
    bool changed = false;

    ImagePart* copyPart(const QPoint& point1, const QPoint& point2) const;
    void pastePart(const ImagePart* part, const QPoint& point);

public:
    int load(const QString& imagePath, QWidget* parent = nullptr);
    int save(const QString& imagePath);
    int getWidth() const;
    int getHeight() const;
    png_byte getBitDepth() const;
    png_byte getColorType() const;
    QString getColorModelName() const;
    QFileInfo getFileInfo() const;
    QPixmap getPixmap() const;
    bool isLoaded() const;
    bool isChanged() const;
    bool isCoordinatesCorrect(const QPoint& point) const;
    void updateFileInfo(const QString& path = "");
    int getCorrectY(int y) const;
    int getCorrectX(int x) const;
    QPoint& correctPoint(QPoint& point) const;
    QPoint correctPoint(const QPoint& point) const;

    void paintSquare(const QPoint& point, int lineWidth, int squareSide, const QColor& color1, FillType fillType, const QColor& color2);
    void changeImageAreas(const QPoint& point1, const QPoint& point2, ExchangingMethod exchangingMethod);
    void replaceCommonColor(const QColor& newColor);
    void invertColors(const QPoint& point1, const QPoint& point2);

    ~ImagePNG();
};


struct ImagePNG::ImagePart {
    int width = 0;
    int height = 0;
    png_bytep* pixelArray = nullptr;

    ImagePart(int width, int height);
    ~ImagePart();
};


#endif // IMAGEPNG_H
