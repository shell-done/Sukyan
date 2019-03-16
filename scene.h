#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QVector2D>
#include "core.h"
#include "polygon.h"
#include "polygonslist.h"

class Scene : public QGraphicsScene {
    Q_OBJECT

public:
	Scene(QPixmap* pxm, PolygonsList* polyList);
	~Scene();

    void setMode(Sukyan::mode mode);
    void setTool(Sukyan::drawingTool tool);
    void setPen(QPen pen);
	void show(bool s);
	void clear();
	bool savePolygons(QWidget*);
	void importPolygons(QWidget*);

	bool getImageSaved();
	bool getPolygonsSaved();

	const QPixmap* getDrawingPixmap();
	QPixmap getPolygonsPixmap();
	PolygonsList* getPolygonsList();

private:
    QGraphicsPixmapItem* backgroundGraphicsPixmap;

	bool imageSaved = true;
	bool pointGrabbed = false;
	QVector2D grabbedPointDist;

    Sukyan::mode mode;
    Sukyan::drawingTool tool;
    Qt::MouseButton buttonPressed;

    QPen pen;

    QPoint firstClick;
    QGraphicsEllipseItem* penCursor;

    QPixmap currentDrawingPixmap;
    QGraphicsPixmapItem* currentDrawingGraphicsPixmap;

    QPixmap drawingPixmap;
    QGraphicsPixmapItem* drawingGraphicsPixmap;

	PolygonsList* polygons;
	QList<QGraphicsPolygonItem*> graphicsPolygons;
	QList<QList<QGraphicsEllipseItem*>> graphicsPolygonsPoint;

    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

    void useBucket(QPoint);

private slots:
	void updateScene();
};

#endif // SCENE_H
