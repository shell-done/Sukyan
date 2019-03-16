#include "scene.h"

Scene::Scene(QPixmap* pxm, PolygonsList* polyList) : QGraphicsScene (0, 0, 100, 100), currentDrawingPixmap(pxm->size()), drawingPixmap(pxm->size()) {
	backgroundGraphicsPixmap = this->addPixmap(*pxm);
    setSceneRect(-100, -100, pxm->width() + 200, pxm->height() + 200);

    drawingPixmap.fill(Qt::transparent);
    drawingGraphicsPixmap = this->addPixmap(drawingPixmap);

    currentDrawingPixmap.fill(Qt::transparent);
    currentDrawingGraphicsPixmap = this->addPixmap(currentDrawingPixmap);

    mode = Sukyan::CURSOR;
    tool = Sukyan::PEN;

    pen.setColor(QColor(0, 0, 0));
    pen.setWidth(10);

    QPen cursorPen;
    cursorPen.setColor(QColor(180, 180, 180));
    cursorPen.setStyle(Qt::DotLine);
    penCursor = this->addEllipse(0, 0, 0, 0);
    penCursor->setPen(cursorPen);

	buttonPressed = Qt::NoButton;

	graphicsPolygons << this->addPolygon(polyList->at(0));
	polygons = polyList;
	graphicsPolygonsPoint << QList<QGraphicsEllipseItem*>();
	connect(polygons, SIGNAL(updtScene()), this, SLOT(updateScene()));
}

Scene::~Scene() {
	delete polygons;
}

void Scene::setMode(Sukyan::mode m) {mode = m;}
void Scene::setTool(Sukyan::drawingTool t) {tool = t;}
void Scene::setPen(QPen p) {pen = p;}
void Scene::show(bool s) {
	drawingGraphicsPixmap->setVisible(s);
	currentDrawingGraphicsPixmap->setVisible(s);
}
void Scene::clear() {
	drawingPixmap.fill(Qt::transparent);
	drawingGraphicsPixmap->setPixmap(drawingPixmap);
}

bool Scene::savePolygons(QWidget* parent) {
	return polygons->exportCSV(parent);
}
void Scene::importPolygons(QWidget* parent) {
	polygons->importCSV(parent);
}

bool Scene::getImageSaved() {return imageSaved;}
bool Scene::getPolygonsSaved() {return polygons->saved();}

const QPixmap* Scene::getDrawingPixmap() {
	return &drawingPixmap;
}

QPixmap Scene::getPolygonsPixmap() {
	QPixmap pxmap(drawingPixmap.size());
	pxmap.fill(Qt::transparent);

	QPainter p(&pxmap);
	QPen linePen(QColor(0, 0, 0));
	linePen.setWidth(2);

	for(int i=0; i<polygons->size(); i++) {
		if(!polygons->at(i).getVisibility())
			continue;

		linePen.setColor(polygons->at(i).getColor());
		p.setPen(linePen);
		p.setBrush(Qt::NoBrush);
		p.drawPolygon(polygons->at(i));

		p.setPen(Qt::NoPen);
		p.setBrush(QBrush(polygons->at(i).getColor()));
		for(int j=0; j<polygons->at(i).size(); j++)
			p.drawEllipse(polygons->at(i).at(j), 5, 5);
	}

	p.end();
	return pxmap;
}

PolygonsList* Scene::getPolygonsList() {return polygons;}


void Scene::updateScene() {
	while(graphicsPolygons.size()) {
		QGraphicsPolygonItem* p = graphicsPolygons.takeAt(0);
		removeItem(p);
		delete p;

		QList<QGraphicsEllipseItem*> l = graphicsPolygonsPoint.takeAt(0);
		while(l.count()) {
			QGraphicsEllipseItem* e = l.takeAt(0);
			removeItem(e);
			delete e;
		}
	}
	graphicsPolygons.clear();
	graphicsPolygonsPoint.clear();


	for(int i=0; i<polygons->size(); i++) {
		QGraphicsPolygonItem* p = this->addPolygon(polygons->at(i));
		QPen pen(polygons->at(i).getColor());
		pen.setWidth(1);
		p->setPen(pen);
		p->setVisible(polygons->at(i).getVisibility());

		QList<QGraphicsEllipseItem*> e;
		for(int j=0; j<polygons->at(i).size(); j++) {
			QRectF rect(polygons->at(i).at(j) - QPointF(1, 1), QSize(2, 2));
			e << this->addEllipse(rect);

			e[j]->setBrush(QBrush(polygons->at(i).getColor()));
			if(i == polygons->getCurrentIndex())
				if(j == polygons->at(i).getCurrentPointIdx())
					e[j]->setBrush(QBrush(Qt::red));

			e[j]->setPen(QPen(Qt::transparent));
			e[j]->setVisible(polygons->at(i).getVisibility());
			if(polygons->at(i).getVisibility()) {
				e[j]->setCursor(Qt::OpenHandCursor);

				if(i == polygons->getCurrentIndex() && j == polygons->getCurrentPolygonPointIndex() && pointGrabbed)
					e[j]->setCursor(Qt::ClosedHandCursor);
			}
		}
		graphicsPolygonsPoint << e;

		graphicsPolygons << p;
	}
}


void Scene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(mode == Sukyan::CURSOR)
        return;

	pointGrabbed = false;
	if(mode == Sukyan::POLYGON) {
		if(polygons->clickOnPoint(event->scenePos(), 1)) {
			pointGrabbed = true;
			grabbedPointDist.setX(event->scenePos().toPoint().x() - polygons->getCurrentPolygonPoint().x());
			grabbedPointDist.setY(event->scenePos().toPoint().y() - polygons->getCurrentPolygonPoint().y());


			return;
		}
		polygons->appendPointToCurrentPolygon(event->scenePos().toPoint());
		return;
	}

    QPainter painter(&drawingPixmap);
    painter.setPen(pen);

    buttonPressed = event->button();

    if(tool == Sukyan::PEN || tool == Sukyan::RUBBER) {
        if(tool == Sukyan::RUBBER || buttonPressed == Qt::RightButton)
            painter.setCompositionMode(QPainter::CompositionMode_Clear);

        painter.setPen(QPen(pen.color()));
        painter.setBrush(QBrush(pen.color()));
        painter.drawEllipse(event->scenePos(), pen.width()/2, pen.width()/2);
        painter.end();

    } else if(tool == Sukyan::LINE || tool == Sukyan::RECTANGLE || tool == Sukyan::ELLIPSE) {
        firstClick = event->scenePos().toPoint();
        painter.end();

    } else {
        painter.end();
        useBucket(event->scenePos().toPoint());
    }
	imageSaved = false;

    drawingGraphicsPixmap->setPixmap(drawingPixmap);

    QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
	if(mode == Sukyan::CURSOR)
        return;

	if(mode == Sukyan::POLYGON) {
		if(pointGrabbed) {
			QPoint p;
			p.setX(event->scenePos().toPoint().x() - static_cast<int>(grabbedPointDist.x()));
			p.setY(event->scenePos().toPoint().y() - static_cast<int>(grabbedPointDist.y()));
			polygons->setCurrentPolygonPoint(p);

			QGraphicsEllipseItem* e = graphicsPolygonsPoint[polygons->getCurrentIndex()][polygons->getCurrentPolygonPointIndex()];
			if(e->isVisible())
				e->setCursor(Qt::ClosedHandCursor);
		}
		return;
	}

    QPoint widthRect(pen.width()/2, pen.width()/2);
    penCursor->setRect(QRect(event->scenePos().toPoint() - widthRect, event->scenePos().toPoint() + widthRect));

    if(buttonPressed != Qt::LeftButton && buttonPressed != Qt::RightButton)
        return;

    currentDrawingPixmap.fill(Qt::transparent);

    if(tool == Sukyan::PEN || tool == Sukyan::RUBBER) {
        QPainter painter(&drawingPixmap);
        painter.setPen(pen);

        QPen linePen = pen;
        linePen.setCapStyle(Qt::RoundCap);
        if(tool == Sukyan::RUBBER || buttonPressed == Qt::RightButton)
            painter.setCompositionMode(QPainter::CompositionMode_Clear);

        painter.setPen(linePen);
        painter.drawLine(event->lastScenePos(), event->scenePos());

        painter.end();
        drawingGraphicsPixmap->setPixmap(drawingPixmap);

    } else if(tool == Sukyan::LINE || tool == Sukyan::RECTANGLE || tool == Sukyan::ELLIPSE) {
        QPainter painter(&currentDrawingPixmap);
        painter.setPen(pen);

        if(tool == Sukyan::LINE)
            painter.drawLine(firstClick, event->scenePos().toPoint());
        else if(tool == Sukyan::RECTANGLE)
            painter.drawRect(QRect(firstClick, event->scenePos().toPoint()));
        else if(tool == Sukyan::ELLIPSE)
            painter.drawEllipse(QRect(firstClick, event->scenePos().toPoint()));

        painter.end();
        currentDrawingGraphicsPixmap->setPixmap(currentDrawingPixmap);
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsScene::mouseReleaseEvent(event);
    buttonPressed = Qt::NoButton;
	pointGrabbed = false;

    if(tool == Sukyan::LINE || tool == Sukyan::RECTANGLE || tool == Sukyan::ELLIPSE) {
        QPainter painter(&drawingPixmap);
        painter.drawPixmap(0, 0, currentDrawingPixmap);

        currentDrawingPixmap.fill(Qt::transparent);
    }

    drawingGraphicsPixmap->setPixmap(drawingPixmap);
    currentDrawingGraphicsPixmap->setPixmap(currentDrawingPixmap);
}

void Scene::useBucket(QPoint p) {
    QImage drawingImage = drawingPixmap.toImage();
    QColor target = drawingImage.pixelColor(p);
    if(target == pen.color())
        return;

    QList<QPoint> pxList;
    pxList << p;

    while(pxList.size()) {
        p = pxList.takeFirst();
        drawingImage.setPixelColor(p, pen.color());

        for(int i=-1; i<=1; i++) {
            for(int j=-1; j<=1; j++) {
                int x = p.x() + i;
                int y = p.y() + j;

                if(qAbs(i) == qAbs(j))
                    continue;

                if(x < 0 || x >= drawingImage.width() || y < 0 || y >= drawingImage.height())
                    continue;

                if(drawingImage.pixelColor(x, y) == target)
                    if(!pxList.contains(QPoint(x, y)))
                        pxList << QPoint(x, y);
            }
        }
    }

    QPainter painter(&drawingPixmap);
    painter.drawPixmap(0, 0, QPixmap::fromImage(drawingImage));
}
