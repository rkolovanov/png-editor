#ifndef IMAGETOOLBAR_H
#define IMAGETOOLBAR_H

#include <QToolBar>
#include <QSpinBox>
#include <QToolButton>
#include "enumerations.h"


class ImageToolBar: public QToolBar {
    Q_OBJECT

private:
    QColor color1 = QColor(Qt::black);
    QColor color2 = QColor(Qt::white);
    ToolType selectedTool = ToolType::NONE_TOOL;
    ExchangingMethod selectedExchangingMethod = ExchangingMethod::CLOCKWISE_METHOD;
    FillType selectedFillType = FillType::UNFILLED;

public:
    ImageToolBar(QWidget* parent = nullptr);

    QColor getFirstColor() const;
    QColor getSecondColor() const;
    ToolType getSelectedTool() const;
    ExchangingMethod getExchangingMethod() const;
    FillType getFillType() const;
    void setFirstColor(const QColor& color);
    void setSecondColor(const QColor& color);
    void setToolType(ToolType newToolType);
    void setExchangingMethod(ExchangingMethod newExchangingMethod);
    void setFillType(FillType newFillType);
    void resetTools();

    QToolButton* changeColor1_button = nullptr;
    QToolButton* changeColor2_button = nullptr;
    QToolButton* changeTool1_button = nullptr;
    QToolButton* changeTool2_button = nullptr;
    QToolButton* changeTool3_button = nullptr;
    QToolButton* changeTool4_button = nullptr;
    QToolButton* changeSquarePainting_button = nullptr;
    QToolButton* changeExchangeMethod_button = nullptr;
    QSpinBox* lineWidthSpinBox = nullptr;
    QSpinBox* squareSizeSpinBox = nullptr;
};

#endif // IMAGETOOLBAR_H
