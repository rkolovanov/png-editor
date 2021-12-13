#include "imagetoolbar.h"
#include "utilityfunctions.h"
#include <QToolButton>
#include <QLabel>


ImageToolBar::ImageToolBar(QWidget *parent): QToolBar(parent) {
    setStyleSheet("QLabel {qproperty-alignment: AlignCenter;font-size: 8px;} QToolButton {padding: 0px; margin: 0px 1px 0px 1px; width: 40px;} QSpinBox {height: 17px; width: 20px;}");
    setContextMenuPolicy(Qt::PreventContextMenu);
    setObjectName("toolbar");
    setMovable(false);
    setIconSize(QSize(40, 17));

    changeColor1_button = createToolButtonColor(this, "changeColor1", "Color 1", QColor(Qt::black), false);
    changeColor2_button = createToolButtonColor(this, "changeColor2", "Color 2", QColor(Qt::white), false);
    changeTool1_button = createToolButton(this, "changeTool1", "Drawing a square", ":/icons/square_tool.png", true);
    changeTool2_button = createToolButton(this, "changeTool2", "Exchange 4 parts of the selected area", ":/icons/exchanging_tool.png", true);
    changeTool3_button = createToolButton(this, "changeTool3", "Replace a common color", ":/icons/common_color_tool.png", false);
    changeTool4_button = createToolButton(this, "changeTool4", "Color inversion in the selected area", ":/icons/invert_color.png", true);
    changeSquarePainting_button = createToolButton(this, "changeSquarePainting", "Square filling", ":/icons/square_unpainted.png", false);
    changeExchangeMethod_button = createToolButton(this, "changeExchangeMethod", "Exchanging parts method", ":/icons/exchanging_1.png", false);

    lineWidthSpinBox = createSpinBox(this, 5, 0, 1000000, "Line width");
    squareSizeSpinBox = createSpinBox(this, 20, 0, 1000000, "Square side");

    QLabel* label1 = new QLabel("COLORS:", this);
    QLabel* label2 = new QLabel("LINE WIDTH:", this);
    QLabel* label3 = new QLabel("SQUARE\nSIDE:", this);

    addWidget(label1);
    addWidget(changeColor1_button);
    addWidget(changeColor2_button);
    addSeparator();
    addWidget(label2);
    addWidget(lineWidthSpinBox);
    addSeparator();
    addWidget(label3);
    addWidget(squareSizeSpinBox);
    addSeparator();
    addWidget(changeTool1_button);
    addWidget(changeSquarePainting_button);
    addSeparator();
    addWidget(changeTool2_button);
    addWidget(changeExchangeMethod_button);
    addSeparator();
    addWidget(changeTool3_button);
    addSeparator();
    addWidget(changeTool4_button);
}

QColor ImageToolBar::getFirstColor() const {
    return color1;
}

QColor ImageToolBar::getSecondColor() const {
    return color2;
}

void ImageToolBar::setFirstColor(const QColor& color) {
    color1 = color;
}

void ImageToolBar::setSecondColor(const QColor& color) {
    color2 = color;
}

void ImageToolBar::resetTools() {
    selectedTool = ToolType::NONE_TOOL;

    changeTool1_button->setChecked(false);
    changeTool2_button->setChecked(false);
    changeTool4_button->setChecked(false);
}

ToolType ImageToolBar::getSelectedTool() const {
    return selectedTool;
}

ExchangingMethod ImageToolBar::getExchangingMethod() const {
    return selectedExchangingMethod;
}

FillType ImageToolBar::getFillType() const {
    return selectedFillType;
}

void ImageToolBar::setToolType(ToolType newToolType) {
    selectedTool = newToolType;
}

void ImageToolBar::setExchangingMethod(ExchangingMethod newExchangingMethod) {
    selectedExchangingMethod = newExchangingMethod;
}

void ImageToolBar::setFillType(FillType newFillType) {
    selectedFillType = newFillType;
}
