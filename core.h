#ifndef CORE_H
#define CORE_H

#include <QtWidgets>
#include <QtGlobal>
#include <QDebug>

namespace Sukyan {
    typedef struct cursorInformation {
        QPoint position;
        QColor pixelColor;
    } cursorInformation;

    typedef struct matrixInformation {
        int scale;
        int rotation;
    } matrixInformation;

    typedef enum mode {CURSOR, DRAWING, POLYGON} mode;
    typedef enum drawingTool {PEN, LINE, RECTANGLE, ELLIPSE, BUCKET, RUBBER} drawingTool;
};

//@TODO :
//Add button to enable/disable antialiasing and smooth transform

#endif // CORE_H
