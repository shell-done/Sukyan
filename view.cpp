#include "view.h"

View::View(Scene* scn) : QGraphicsView(scn), scene(scn) {
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setMouseTracking(true);
    setMinimumWidth(400);
}

void View::rotationChanged(int r) {
    rotationFactor = r;
    updateMatrix();
}

void View::zoomChanged(int s) {
    scaleFactor = s/100.;
    updateMatrix();
}

Sukyan::matrixInformation View::getMatrix() {
    Sukyan::matrixInformation mtrxInfo;
    mtrxInfo.scale = static_cast<int>(scaleFactor*100);
    mtrxInfo.rotation = rotationFactor;

    return mtrxInfo;
}

void View::wheelEvent(QWheelEvent* event) {
    this->setTransformationAnchor(AnchorUnderMouse);

    double s = event->delta() > 0 ? 1.25 : 0.8;

    scaleFactor *= s;
    scaleFactor = qMin(scaleFactor, static_cast<double>(MAX_ZOOM));
    scaleFactor = qMax(scaleFactor, static_cast<double>(MIN_ZOOM));

    emit wheelZoomValueChanged(static_cast<int>(scaleFactor*100));
    updateMatrix();
}

void View::mouseMoveEvent(QMouseEvent* event) {
    emit mousePositionChanged(mapToScene(event->pos()).toPoint());
    QGraphicsView::mouseMoveEvent(event);
}



void View::updateMatrix() {
    QMatrix matrix;
    matrix.scale(scaleFactor, scaleFactor);
    matrix.rotate(rotationFactor);

    setMatrix(matrix);
}

