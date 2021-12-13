#include "imagepng.h"
#include "utilityfunctions.h"
#include <cmath>
#include <QMessageBox>
#include <QDebug>


int ImagePNG::load(const QString& imagePath, QWidget* parent) {
    FILE* file = fopen(imagePath.toStdString().c_str(), "rb");

    if (!file) {
        qDebug("Error in ImagePNG::load with fopen()");
        return 1;
    }

    unsigned char header[8];
    fread(header, sizeof(unsigned char), 8, file);

    if (png_sig_cmp(header, 0, 8)){
        qDebug("Error in ImagePNG::load with png_sig_cmp() (Non-PNG header)");
        fclose(file);
        return 2;
    }

    png_structp pngStructPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!pngStructPtr) {
        qDebug("Error in ImagePNG::load with png_create_read_struct()");
        fclose(file);
        return 3;
    }

    png_infop pngInfoStructPtr = png_create_info_struct(pngStructPtr);

    if (!pngInfoStructPtr) {
        qDebug("Error in ImagePNG::load with png_create_info_struct()");
        png_destroy_read_struct(&pngStructPtr, NULL, NULL);
        fclose(file);
        return 4;
    }

    if (setjmp(png_jmpbuf(pngStructPtr))) {
        qDebug("Error in ImagePNG::load with setjmp(png_jmpbuf())");
        png_destroy_read_struct(&pngStructPtr, &pngInfoStructPtr, NULL);
        fclose(file);
        return 5;
    }

    png_init_io(pngStructPtr, file);
    png_set_sig_bytes(pngStructPtr, 8);
    png_read_info(pngStructPtr, pngInfoStructPtr);

    width = png_get_image_width(pngStructPtr, pngInfoStructPtr);
    height = png_get_image_height(pngStructPtr, pngInfoStructPtr);
    colorType = png_get_color_type(pngStructPtr, pngInfoStructPtr);
    bitDepth = png_get_bit_depth(pngStructPtr, pngInfoStructPtr);
    filteringMethod = png_get_filter_type(pngStructPtr, pngInfoStructPtr);
    compressionType = png_get_compression_type(pngStructPtr, pngInfoStructPtr);
    interlaceType = png_get_interlace_type(pngStructPtr, pngInfoStructPtr);

    png_set_interlace_handling(pngStructPtr);

    if (compressionType != 0 || filteringMethod != 0) {
        qDebug("This image format is not supported");
        png_destroy_read_struct(&pngStructPtr, &pngInfoStructPtr, NULL);
        fclose(file);
        return 9;
    }

    qDebug("Color type: %d, bit depth: %d, interlace type: %d", colorType, bitDepth, interlaceType);

    if (bitDepth != 8 || colorType != PNG_COLOR_TYPE_RGBA) {
        QMessageBox::StandardButton button = QMessageBox::question(parent, "Converting an image", "The selected PNG image doesn't match the RGBA 8-bit format. Do you want to convert this image to this format?");

        if (button == QMessageBox::StandardButton::No) {
            qDebug("This image format is not supported");
            png_destroy_read_struct(&pngStructPtr, &pngInfoStructPtr, NULL);
            fclose(file);
            return 8;
        }
    }

    if (bitDepth == 16) {
        qDebug("Converting bit depth: 16 to 8");
        png_set_strip_16(pngStructPtr);
        bitDepth = 8;
    }

    if (bitDepth < 8) {
        qDebug("Converting bit depth: 1, 2 or 4 to 8");

        if (colorType == PNG_COLOR_TYPE_GRAY) {
            png_set_expand_gray_1_2_4_to_8(pngStructPtr);
        } else {
            png_set_packing(pngStructPtr);
        }

        bitDepth = 8;
    }

    if (colorType == PNG_COLOR_TYPE_PALETTE) {
        qDebug("Converting color type: COLOR_TYPE_PALETTE to COLOR_TYPE_RGB");
        png_set_palette_to_rgb(pngStructPtr);
        colorType = PNG_COLOR_TYPE_RGB;
    }

    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        qDebug("Converting color type: PNG_COLOR_TYPE_GRAY to COLOR_TYPE_RGB or PNG_COLOR_TYPE_GRAY_ALPHA to COLOR_TYPE_RGBA");
        png_set_gray_to_rgb(pngStructPtr);

        if (colorType == PNG_COLOR_TYPE_GRAY) {
            colorType = PNG_COLOR_TYPE_RGB;
        } else {
            colorType = PNG_COLOR_TYPE_RGBA;
        }
    }

    if (colorType == PNG_COLOR_TYPE_RGB) {
        qDebug("Converting color type: COLOR_TYPE_RGB to COLOR_TYPE_RGBA, fill alpha channel");
        png_set_add_alpha(pngStructPtr, 0xFF, PNG_FILLER_AFTER);
        colorType = PNG_COLOR_TYPE_RGBA;
    }

    if (png_get_valid(pngStructPtr, pngInfoStructPtr, PNG_INFO_tRNS)) {
        qDebug("Converting color type to alpha");
        png_set_tRNS_to_alpha(pngStructPtr);
        colorType = PNG_COLOR_TYPE_RGBA;
    }

    png_read_update_info(pngStructPtr, pngInfoStructPtr);

    colorType = png_get_color_type(pngStructPtr, pngInfoStructPtr);
    bitDepth = png_get_bit_depth(pngStructPtr, pngInfoStructPtr);

    qDebug("Converted color type: %d, bit depth: %d", colorType, bitDepth);

    if (colorType != PNG_COLOR_TYPE_RGBA || bitDepth != 8) {
        qDebug("Error with converting image to RGBA 8 bit");
        png_destroy_read_struct(&pngStructPtr, &pngInfoStructPtr, NULL);
        fclose(file);
        return 6;
    }

    if (setjmp(png_jmpbuf(pngStructPtr))) {
        qDebug("Error in ImagePNG::load with setjmp(png_jmpbuf()) #2");
        png_destroy_read_struct(&pngStructPtr, &pngInfoStructPtr, NULL);
        fclose(file);
        return 7;
    }

    pixelArray = new png_bytep[height];

    for (int y = 0; y < height; y++) {
        pixelArray[y] = new png_byte[png_get_rowbytes(pngStructPtr, pngInfoStructPtr)];
    }

    png_read_image(pngStructPtr, pixelArray);
    qDebug("The image is successfully loaded: %s", imagePath.toStdString().c_str());

    png_destroy_read_struct(&pngStructPtr, &pngInfoStructPtr, NULL);
    fclose(file);

    loaded = true;
    fileInfo.setFile(imagePath);

    return 0;
}

int ImagePNG::save(const QString& imagePath) {
    FILE* file = fopen(imagePath.toStdString().c_str(), "wb");

    if (!file) {
        qDebug("Error in ImagePNG::save with fopen()");
        return 1;
    }

    png_structp pngStructPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngStructPtr) {
        qDebug("Error in ImagePNG::save with png_create_write_struct()");
        fclose(file);
        return 2;
    }

    png_infop pngInfoStructPtr = png_create_info_struct(pngStructPtr);
    if (!pngInfoStructPtr) {
        qDebug("Error in ImagePNG::save with png_create_info_struct()");
        png_destroy_write_struct(&pngStructPtr, NULL);
        fclose(file);
        return 3;
    }

    if (setjmp(png_jmpbuf(pngStructPtr))){
        qDebug("Error in ImagePNG::save with setjmp(png_jmpbuf())");
        png_destroy_write_struct(&pngStructPtr, &pngInfoStructPtr);
        fclose(file);
        return 4;
    }

    png_init_io(pngStructPtr, file);

    if (setjmp(png_jmpbuf(pngStructPtr))){
        qDebug("Error in ImagePNG::save with setjmp(png_jmpbuf()) #2");
        png_destroy_write_struct(&pngStructPtr, &pngInfoStructPtr);
        fclose(file);
        return 5;
    }

    png_set_IHDR(pngStructPtr, pngInfoStructPtr, width, height, bitDepth, colorType, interlaceType, compressionType, filteringMethod);
    png_write_info(pngStructPtr, pngInfoStructPtr);

    if (setjmp(png_jmpbuf(pngStructPtr))){
        qDebug("Error in ImagePNG::save with setjmp(png_jmpbuf()) #3");
        png_destroy_write_struct(&pngStructPtr, &pngInfoStructPtr);
        fclose(file);
        return 6;
    }

    png_write_image(pngStructPtr, pixelArray);

    if (setjmp(png_jmpbuf(pngStructPtr))){
        qDebug("Error in ImagePNG::save with setjmp(png_jmpbuf()) #4");
        png_destroy_write_struct(&pngStructPtr, &pngInfoStructPtr);
        fclose(file);
        return 7;
    }

    png_write_end(pngStructPtr, NULL);

    qDebug("The image is successfully saved: %s", imagePath.toStdString().c_str());

    png_destroy_write_struct(&pngStructPtr, &pngInfoStructPtr);
    fclose(file);

    changed = false;

    return 0;
}

ImagePNG::ImagePart* ImagePNG::copyPart(const QPoint& point1, const QPoint& point2) const {
    if (isLoaded() && isCoordinatesCorrect(point1) && isCoordinatesCorrect(point2)) {
        ImagePart* part = new ImagePart(point2.x() - point1.x() + 1, point2.y() - point1.y() + 1);

        for (int y = 0; y < part->height; y++) {
            for (int x = 0; x < part->width; x++) {
                setPixel(part->pixelArray[y] + 4 * x, getPixel(pixelArray[point1.y() + y] + 4 * (point1.x() + x)));
            }
        }

        qDebug("Part of the image in area %s to %s was successfully copied", pointToString(point1).toStdString().c_str(), pointToString(point2).toStdString().c_str());

        return part;
    }

    return nullptr;
}

void ImagePNG::pastePart(const ImagePNG::ImagePart* part, const QPoint& point) {
    if (isLoaded() && part != nullptr && isCoordinatesCorrect(point)) {
        int endY = getCorrectY(point.y() + part->height);
        int endX = getCorrectX(point.x() + part->width);

        for (int y = point.y(); y < endY; y++) {
            for (int x = point.x(); x < endX; x++) {
                setPixel(pixelArray[y] + 4 * x, getPixel(part->pixelArray[y - point.y()] + 4 * (x - point.x())));
            }
        }

        qDebug("Part of the image was successfully pasted in %s", pointToString(point).toStdString().c_str());
    }
}

int ImagePNG::getWidth() const {
    return width;
}

int ImagePNG::getHeight() const {
    return height;
}

png_byte ImagePNG::getBitDepth() const {
    return bitDepth;
}

png_byte ImagePNG::getColorType() const {
    return colorType;
}

QFileInfo ImagePNG::getFileInfo() const {
    return fileInfo;
}

QString ImagePNG::getColorModelName() const {
    switch (colorType) {
    case PNG_COLOR_TYPE_RGB:
        return "RGB";
    case PNG_COLOR_TYPE_GRAY:
        return "Gray";
    case PNG_COLOR_TYPE_RGBA:
        return "RGBA";
    case PNG_COLOR_TYPE_PALETTE:
        return "Palette";
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        return "Gray alpha";
    default:
        return "Unknown";
    }
}

QPixmap ImagePNG::getPixmap() const {
    QPixmap pixmap;

    if (isLoaded())  {
        QImage image(width, height, QImage::Format_RGBA64);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                png_bytep pixel = pixelArray[y] + 4 * x;
                image.setPixelColor(x, y, QColor(pixel[0], pixel[1], pixel[2], pixel[3]));
            }
        }

        pixmap = QPixmap::fromImage(image);
    }

    return pixmap;
}

bool ImagePNG::isLoaded() const {
    return loaded && pixelArray != nullptr;
}

bool ImagePNG::isChanged() const {
    return changed;
}

bool ImagePNG::isCoordinatesCorrect(const QPoint& point) const {
    return (point.x() >= 0 && point.y() >= 0 && point.x() < width && point.y() < height);
}

void ImagePNG::updateFileInfo(const QString& path) {
    if (path == "") {
        fileInfo = QFileInfo(fileInfo.filePath());
    } else {
        fileInfo = QFileInfo(path);
    }
}

int ImagePNG::getCorrectY(int y) const {
    if (y < 0) {
        return 0;
    }

    if (y >= height) {
        return height - 1;
    }

    return y;
}

int ImagePNG::getCorrectX(int x) const {
    if (x < 0) {
        return 0;
    }

    if (x >= width) {
        return width - 1;
    }

    return x;
}

QPoint& ImagePNG::correctPoint(QPoint& point) const {
    if (!isCoordinatesCorrect(point)) {
        point.setX(getCorrectX(point.x()));
        point.setY(getCorrectY(point.y()));
    }

    return point;
}

QPoint ImagePNG::correctPoint(const QPoint& point) const {
    return QPoint(getCorrectX(point.x()), getCorrectY(point.y()));
}

void ImagePNG::paintSquare(const QPoint& point, int lineWidth, int squareSide, const QColor& color1, FillType fillType, const QColor& color2) {
    if (!isLoaded()) {
        return;
    }

    qDebug("Starting drawing a square with %d line width and %d square side:", lineWidth, squareSide);
    qDebug("Starting point: %s;", pointToString(point).toStdString().c_str());
    qDebug("Filling type: %d;", fillType);
    qDebug("Colors: %s and %s.", colorToString(color1).toStdString().c_str(), colorToString(color2).toStdString().c_str());

    int xRight = getCorrectX(point.x() + squareSide - 1);
    int xInsideRight = point.x() + squareSide - lineWidth - 1;
    int xInsideLeft = point.x() + lineWidth;
    int yDown = getCorrectY(point.y() + squareSide - 1);
    int yIndiseUp = point.y() + lineWidth;
    int yIndiseDown = point.y() + squareSide - lineWidth - 1;


    for (int y = point.y(); y <= yDown; y++) {
        for (int x = point.x(); x <= xRight; x++) {
            if (isCoordinatesCorrect(QPoint(x, y))) {
                if (x < xInsideLeft || x > xInsideRight || y < yIndiseUp || y > yIndiseDown) {
                    setPixel(pixelArray[y] + 4 * x, color1);
                } else if (fillType == FillType::FILLED) {
                    setPixel(pixelArray[y] + 4 * x, color2);
                }
            }
        }
    }

    qDebug("Drawing a square finished");

    changed = true;
}

void ImagePNG::changeImageAreas(const QPoint& point1, const QPoint& point2, ExchangingMethod exchangingMethod) {
    if (!isLoaded()) {
        return;
    }

    QPoint fixedPoint1 = correctPoint(point1);
    QPoint fixedPoint2 = correctPoint(point2);

    if ((fixedPoint2.x() - fixedPoint1.x()) % 2 == 0) {
        fixedPoint2.setX(point2.x() - 1);
    }

    if ((fixedPoint2.y() - fixedPoint1.y()) % 2 == 0) {
        fixedPoint2.setY(point2.y() - 1);
    }

    int deltaX = fixedPoint2.x() - fixedPoint1.x();
    int deltaY = fixedPoint2.y() - fixedPoint1.y();
    int partWidth = deltaX / 2 + 1;
    int partHeight = deltaY / 2 + 1;
    int middleX = fixedPoint1.x() + partWidth;
    int middleY = fixedPoint1.y() + partHeight;

    if (partWidth < 2 || partHeight < 2) {
        qDebug("Area is too small!");
        return;
    }

    QPoint partPoint1 = fixedPoint1;
    QPoint partPoint2 = QPoint(middleX, fixedPoint1.y());
    QPoint partPoint3 = QPoint(fixedPoint1.x(), middleY);
    QPoint partPoint4 = QPoint(middleX, middleY);

    ImagePart* part1 = copyPart(partPoint1, QPoint(middleX - 1, middleY - 1));
    ImagePart* part2 = copyPart(partPoint2, QPoint(fixedPoint2.x(), middleY - 1));
    ImagePart* part3 = copyPart(partPoint3, QPoint(middleX - 1, fixedPoint2.y()));
    ImagePart* part4 = copyPart(partPoint4, fixedPoint2);

    if (exchangingMethod == ExchangingMethod::CLOCKWISE_METHOD) {
        pastePart(part1, partPoint2);
        pastePart(part2, partPoint4);
        pastePart(part3, partPoint1);
        pastePart(part4, partPoint3);
    } else if (exchangingMethod == ExchangingMethod::ANTICLOCKWISE_METHOD) {
        pastePart(part1, partPoint3);
        pastePart(part2, partPoint1);
        pastePart(part3, partPoint4);
        pastePart(part4, partPoint2);
    } else {
        pastePart(part1, partPoint4);
        pastePart(part2, partPoint3);
        pastePart(part3, partPoint2);
        pastePart(part4, partPoint1);
    }

    delete part1;
    delete part2;
    delete part3;
    delete part4;

    changed = true;
}

// Needs for QMap working with QColor
bool operator<(const QColor& a, const QColor& b) {
   return a.redF() < b.redF() || a.greenF() < b.greenF() || a.blueF() < b.blueF() || a.alphaF() < b.alphaF();
}

void ImagePNG::replaceCommonColor(const QColor& newColor) {
    if (!isLoaded()) {
        return;
    }

    QMap<QColor, int> dictionary;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            QColor color = getColorFromPixel(pixelArray[y] + 4 * x);

            if (dictionary.contains(color)) {
                dictionary[color] += 1;
            } else {
                dictionary[color] = 1;
            }
        }
    }

    QColor commonColor = dictionary.begin().key();
    int max = dictionary.begin().value();

    for (auto pair : dictionary.toStdMap()) {
        if (pair.second > max) {
            max = pair.second;
            commonColor = pair.first;
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            png_bytep pixel = pixelArray[y] + 4 * x;
            QColor color = getColorFromPixel(pixel);

            if (color == commonColor) {
                setPixel(pixel, newColor);
            }
        }
    }

    changed = true;
}

void ImagePNG::invertColors(const QPoint& point1, const QPoint& point2) {
    if (!isLoaded()) {
        return;
    }

    QPoint fixedPoint1 = correctPoint(point1);
    QPoint fixedPoint2 = correctPoint(point2);

    for (int y = fixedPoint1.y(); y < fixedPoint2.y(); y++) {
        for (int x = fixedPoint1.x(); x < fixedPoint2.x(); x++) {
            invertPixelColor(pixelArray[y] + 4 * x);
        }
    }

    changed = true;
}

ImagePNG::~ImagePNG() {
    if (pixelArray != nullptr) {
        for(int y = 0; y < height; y++) {
            delete[] pixelArray[y];
        }

        delete[] pixelArray;
    }
}

ImagePNG::ImagePart::ImagePart(int width, int height) {
    this->width = width;
    this->height = height;

    pixelArray = new png_bytep[height];

    for (int y = 0; y < height; y++) {
        pixelArray[y] = new png_byte[4 * width];
    }
}

ImagePNG::ImagePart::~ImagePart() {
    if (pixelArray != nullptr) {
        for (int y = 0; y < height; y++) {
            delete[] pixelArray[y];
        }

        delete[] pixelArray;
    }
}
