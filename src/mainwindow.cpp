#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utilityfunctions.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGridLayout>
#include <QDateTime>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    imageViewWidget = new ImageView(this);
    toolbar = new ImageToolBar(this);
    coordinateLabel = new QLabel(pointToString(QPoint(0, 0)), this);
    helpDockWidget = new HelpDockWidget(this);

    ui->setupUi(this);
    ui->statusbar->addPermanentWidget(coordinateLabel);

    addToolBar(Qt::ToolBarArea::LeftToolBarArea, toolbar);
    addDockWidget(Qt::RightDockWidgetArea, helpDockWidget);
    setCentralWidget(imageViewWidget);

    connect(ui->exit, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(quit()));
}

void MainWindow::updateCoordinates(const QPoint& point) {
    coordinateLabel->setText(pointToString(point));
}

void MainWindow::showError(const QString& message) {
    qDebug("%s", message.toStdString().c_str());
    QMessageBox::critical(this, "Error", message);
}

void MainWindow::showStatusMessage(const QString& message, int timeout) {
    qDebug("%s", message.toStdString().c_str());
    ui->statusbar->showMessage(message, timeout);
}

void MainWindow::deleteImage() {
    delete image;
    image = nullptr;
}

MainWindow::~MainWindow() {
    deleteImage();
    delete ui;
}

void MainWindow::on_openFile_triggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "~/",  tr("Image (*.png)"));

    if (filePath == nullptr) {
        return;
    }

    if (image != nullptr && image->isChanged()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, "Saving changes", "Save changes to " + image->getFileInfo().filePath() + " before closing?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

        if (button == QMessageBox::Save) {
            on_saveFile_triggered();
            deleteImage();
            imageViewWidget->clearScene();
        } else if (button == QMessageBox::Cancel) {
            return;
        } else {
            deleteImage();
            imageViewWidget->clearScene();
        }
    }

    image = new ImagePNG;
    int error = image->load(filePath, this);

    if (error == 0) {
        showStatusMessage("File is read: " + filePath);
        imageViewWidget->setImage(image);
    } else {
        switch (error) {
        case 1:
            showError("The file cannot be accessed. Make sure that the file path is correct.");
            break;
        case 2:
            showError("The file doesn't match the PNG format.");
            break;
        case 6:
            showError("This PNG image format is not supported.");
            break;
        default:
            showError("Error opening the image.");
        }
    }
}

void MainWindow::on_saveFile_triggered() {
    if (image != nullptr && image->isLoaded()) {
        int error = image->save(image->getFileInfo().filePath().toStdString().c_str());

        if (error == 0) {
            image->updateFileInfo();
            showStatusMessage("File is saved: " + image->getFileInfo().filePath());
        } else {
            switch (error) {
            case 1:
                showError("The file cannot be accessed. Make sure that the file path is correct.");
                break;
            default:
                showError("Error saving the image.");
            }
        }
    } else {
        showError("The image wasn't loaded.");
    }
}

void MainWindow::on_saveFileAs_triggered() {
    if (image == nullptr || !image->isLoaded()) {
        showError("The image wasn't loaded.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"), "~/",  tr("Image (*.png)"));

    if (filePath == nullptr) {
        return;
    }

    int error = image->save(filePath.toStdString().c_str());

    if (error == 0) {
        image->updateFileInfo(filePath);
        showStatusMessage("File is saved: " + filePath);
    } else {
        switch (error) {
        case 1:
            showError("The file cannot be accessed. Make sure that the file path is correct.");
            break;
        default:
            showError("Error saving the image.");
        }
    }
}

void MainWindow::on_closeFile_triggered() {
    if (image != nullptr && image->isChanged()) {
        QMessageBox::StandardButton button = QMessageBox::question(this, "Saving changes", "Save changes to " + image->getFileInfo().filePath() + " before closing?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

        if (button == QMessageBox::Save) {
            on_saveFile_triggered();
        } else if (button == QMessageBox::Cancel) {
            return;
        }
    }

    if (image != nullptr) {
        deleteImage();
        imageViewWidget->clearScene();
    }
}

void MainWindow::on_changeColor1_clicked() {
    QColor color = QColorDialog::getColor(toolbar->getFirstColor(), this, "Select a color");

    if (color.isValid()) {
        QPixmap pixmap(39, 16);

        pixmap.fill(color);

        toolbar->setFirstColor(color);
        toolbar->changeColor1_button->setIcon(QIcon(pixmap));
    }
}

void MainWindow::on_changeColor2_clicked() {
    QColor color = QColorDialog::getColor(toolbar->getFirstColor(), this, "Select a color");

    if (color.isValid()) {
        QPixmap pixmap(39, 16);

        pixmap.fill(color);

        toolbar->setSecondColor(color);
        toolbar->changeColor2_button->setIcon(QIcon(pixmap));
    }
}

void MainWindow::on_changeTool1_clicked() {
    toolbar->resetTools();

    if (image != nullptr && image->isLoaded()) {
        toolbar->changeTool1_button->setChecked(true);
        toolbar->setToolType(ToolType::SQUARE_PAINT_TOOL);
        showStatusMessage("Tool selected: Square paint");
    } else {
        showError("The image wasn't loaded.");
    }
}

void MainWindow::on_changeTool2_clicked() {
    toolbar->resetTools();

    if (image != nullptr && image->isLoaded()) {
        toolbar->changeTool2_button->setChecked(true);
        toolbar->setToolType(ToolType::EXCHANGING_PARTS_TOOL);
        showStatusMessage("Tool selected: Exchanging parts");
    } else {
        showError("The image wasn't loaded.");
    }
}

void MainWindow::on_changeTool3_clicked() {
    if (image != nullptr && image->isLoaded()) {
        image->replaceCommonColor(toolbar->getFirstColor());
        imageViewWidget->setImage(image);
        showStatusMessage("Operation complete: Change common color");
    } else {
        showError("The image wasn't loaded.");
    }
}

void MainWindow::on_changeTool4_clicked() {
    toolbar->resetTools();

    if (image != nullptr && image->isLoaded()) {
        toolbar->changeTool4_button->setChecked(true);
        toolbar->setToolType(ToolType::INVERT_COLOR_TOOL);
        showStatusMessage("Tool selected: Invert color");
    } else {
        showError("The image wasn't loaded.");
    }
}

void MainWindow::on_changeExchangeMethod_clicked() {
    if (toolbar->getExchangingMethod() == 1) {
        toolbar->changeExchangeMethod_button->setIcon(QIcon(":/icons/exchanging_2.png"));
        toolbar->setExchangingMethod(ExchangingMethod::ANTICLOCKWISE_METHOD);
        showStatusMessage("Exchange method selected: Anticlockwise");
    } else if (toolbar->getExchangingMethod() == 2) {
        toolbar->changeExchangeMethod_button->setIcon(QIcon(":/icons/exchanging_3.png"));
        toolbar->setExchangingMethod(ExchangingMethod::DIAGONAL_METHOD);
        showStatusMessage("Exchange method selected: Diagonal");
    } else {
        toolbar->changeExchangeMethod_button->setIcon(QIcon(":/icons/exchanging_1.png"));
        toolbar->setExchangingMethod(ExchangingMethod::CLOCKWISE_METHOD);
        showStatusMessage("Exchange method selected: Clockwise");
    }
}

void MainWindow::on_changeSquarePainting_clicked() {
    if (toolbar->getFillType() == FillType::UNFILLED) {
        toolbar->changeSquarePainting_button->setIcon(QIcon(":/icons/square_painted.png"));
        toolbar->setFillType(FillType::FILLED);
        showStatusMessage("Square painting selected: Filled");
    } else {
        toolbar->changeSquarePainting_button->setIcon(QIcon(":/icons/square_unpainted.png"));
        toolbar->setFillType(FillType::UNFILLED);
        showStatusMessage("Square painting selected: Unfilled");
    }
}

void MainWindow::on_imageScene_point1Selected(QGraphicsSceneMouseEvent* event) {
    if (event->buttons().testFlag(Qt::MouseButton::LeftButton)) {
        QPoint point = event->scenePos().toPoint();
        int selectedTool = toolbar->getSelectedTool();
        point1 = point;

        if (selectedTool == 1) {
            image->paintSquare(point, toolbar->lineWidthSpinBox->value(), toolbar->squareSizeSpinBox->value(),
                               toolbar->getFirstColor(), toolbar->getFillType(), toolbar->getSecondColor());
            imageViewWidget->setImage(image);
            showStatusMessage("Operation complete: Paint square at " + pointToString(point));
        }
    }
}

void MainWindow::on_imageScene_point2Selected(QGraphicsSceneMouseEvent* event) {
    QPoint point = event->scenePos().toPoint();
    int selectedTool = toolbar->getSelectedTool();

    if (point.x() < point1.x()) {
        int x = point.x();
        point.setX(point1.x());
        point1.setX(x);
    }

    if (point.y() < point1.y()) {
        int y = point.y();
        point.setY(point1.y());
        point1.setY(y);
    }

    if (selectedTool == 4) {
        image->invertColors(point1, point);
        imageViewWidget->setImage(image);
        showStatusMessage("Operation complete: Invert color in area " + pointToString(point1) + " to " + pointToString(point));
    } else if (selectedTool == 2) {
        image->changeImageAreas(point1, point, toolbar->getExchangingMethod());
        imageViewWidget->setImage(image);
        showStatusMessage("Operation complete: Exchange parts in area " + pointToString(point1) + " to " + pointToString(point));
    }
}

void MainWindow::on_imageScene_mousePositionChanged(QGraphicsSceneMouseEvent* event) {
    QPoint point(event->scenePos().toPoint());
    static QPoint previousPosition = QPoint(-100000, -100000);

    if (point != previousPosition) {
        coordinateLabel->setText(pointToString(point));
        previousPosition = point;
    }
}

void MainWindow::on_about_triggered() {
    QMessageBox* aboutBox = new QMessageBox(this);
    aboutBox->setWindowTitle("About PNG Editor");
    aboutBox->setText("<b style='font-size: 18px;'>PNG Editor</b><br>Version: 1.0<br>Author:  Kolovanov Rodion");
    aboutBox->setIcon(QMessageBox::Icon::Information);
    aboutBox->show();
}

void MainWindow::on_help_triggered() {
    helpDockWidget->show();
}

void MainWindow::on_imageInfo_triggered() {
    if (image != nullptr && image->isLoaded()) {
        QString info = "Name: " + image->getFileInfo().fileName() +
                       "\nPath: " + image->getFileInfo().filePath()  +
                       "\nSize: " + QString::number(image->getFileInfo().size()) + " bytes" +
                       "\nLast accessed date: " + image->getFileInfo().lastRead().toString() +
                       "\nWidth: " + QString::number(image->getWidth()) + " px" +
                       "\nHeight: " + QString::number(image->getHeight()) + " px" +
                       "\nColor model: " + image->getColorModelName() + " (" + QString::number(image->getColorType()) + ")" +
                       "\nBit depth: " + QString::number(image->getBitDepth());

        QMessageBox* infoWidget = new QMessageBox(this);
        infoWidget->setWindowTitle("Image Info");
        infoWidget->setText(info);
        infoWidget->setIcon(QMessageBox::Icon::Information);
        infoWidget->show();
    } else {
        showError("The image wasn't loaded.");
    }
}
