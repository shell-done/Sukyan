#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include "core.h"
#include "scene.h"

class View : public QGraphicsView {
    Q_OBJECT

public:
	View(Scene* scene);

    void rotationChanged(int);
    void zoomChanged(int);
    Sukyan::matrixInformation getMatrix();

protected:
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    Scene* scene;
    void updateMatrix();

    static constexpr double MIN_ZOOM = 0.25;
	static constexpr double MAX_ZOOM = 10;

    double scaleFactor = 1.0;
    int rotationFactor = 0;

signals:
    void mousePositionChanged(QPoint position);
    void wheelZoomValueChanged(int);
};


#endif // VIEW_H
