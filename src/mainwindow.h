#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "imageview.h"
#include "imagetoolbar.h"
#include "imagepng.h"
#include "helpbrowser.h"
#include "enumerations.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void updateCoordinates(const QPoint& point);
    void showError(const QString& message);
    void showStatusMessage(const QString& message, int timeout = 0);
    void deleteImage();
    ~MainWindow();

private slots:
    void on_openFile_triggered();
    void on_saveFile_triggered();
    void on_saveFileAs_triggered();
    void on_closeFile_triggered();
    void on_changeColor1_clicked();
    void on_changeColor2_clicked();
    void on_changeTool1_clicked();
    void on_changeTool2_clicked();
    void on_changeTool3_clicked();
    void on_changeTool4_clicked();
    void on_changeExchangeMethod_clicked();
    void on_changeSquarePainting_clicked();
    void on_imageScene_point1Selected(QGraphicsSceneMouseEvent* event);
    void on_imageScene_point2Selected(QGraphicsSceneMouseEvent* event);
    void on_imageScene_mousePositionChanged(QGraphicsSceneMouseEvent* event);
    void on_about_triggered();
    void on_help_triggered();
    void on_imageInfo_triggered();

private:
    Ui::MainWindow* ui = nullptr;
    ImageToolBar* toolbar = nullptr;
    ImageView* imageViewWidget = nullptr;
    HelpDockWidget* helpDockWidget = nullptr;
    ImagePNG* image = nullptr;
    QLabel* coordinateLabel = nullptr;
    QPoint point1;
};


#endif // MAINWINDOW_H
