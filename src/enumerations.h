#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H


enum ToolType {
    NONE_TOOL = 0,
    SQUARE_PAINT_TOOL = 1,
    EXCHANGING_PARTS_TOOL = 2,
    CHANGE_COMMON_COLOR_TOOL = 3,
    INVERT_COLOR_TOOL = 4
};


enum ExchangingMethod {
    CLOCKWISE_METHOD = 1,
    ANTICLOCKWISE_METHOD = 2,
    DIAGONAL_METHOD = 3
};


enum FillType {
    UNFILLED = 1,
    FILLED = 2
};


#endif // ENUMERATIONS_H
