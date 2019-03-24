#ifndef CORE_H
#define CORE_H

#include <QtWidgets>
#include <QtGlobal>
#include <cmath>
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

	typedef enum mode {CURSOR, DRAWING, POLYGON, CONTRAST} mode;
    typedef enum drawingTool {PEN, LINE, RECTANGLE, ELLIPSE, BUCKET, RUBBER} drawingTool;
};

#endif // CORE_H
