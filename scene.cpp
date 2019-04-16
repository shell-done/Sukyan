#include "scene.h"

Scene::Scene(QString fp, PolygonsList* polyList) : QGraphicsScene (0, 0, 100, 100) {
	backgroundPixmap = QPixmap(fp);
	backgroundGraphicsPixmap = this->addPixmap(backgroundPixmap);
	editedBackgroundPixmap = new QPixmap(backgroundPixmap);
	setSceneRect(-100, -100, backgroundPixmap.width() + 200, backgroundPixmap.height() + 200);

	currentDrawingPixmap = QPixmap(backgroundPixmap.size());
	drawingPixmap = QPixmap(backgroundPixmap.size());

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
	buffer = nullptr;
	currentContrast = 0;
	currentBrightness = 0;

	graphicsPolygons << this->addPolygon(polyList->at(0));
	polygons = polyList;
	graphicsPolygonsPoint << QList<QGraphicsEllipseItem*>();
	connect(polygons, SIGNAL(updtScene()), this, SLOT(updateScene()));
}

Scene::~Scene() {
	delete polygons;
	delete buffer;
	delete editedBackgroundPixmap;
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

QPixmap* Scene::getBackgroundPixmap() {
	QPainter p(editedBackgroundPixmap);
	p.drawPixmap(0, 0, backgroundGraphicsPixmap->pixmap());
	p.end();

	return editedBackgroundPixmap;
}

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

void Scene::setContrast(int b, int c) {
	currentBrightness = b;
	currentContrast = c;
	changeContrast(0, 0);
}

void Scene::updateScene() {
	while(graphicsPolygons.size()) {
		QAbstractGraphicsShapeItem* p = graphicsPolygons.takeAt(0);
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
		if(polygons->at(i).getClosedShape()) {
			QGraphicsPolygonItem* p = this->addPolygon(polygons->at(i));
			QPen pen(polygons->at(i).getColor());
			pen.setWidth(1);
			p->setPen(pen);
			p->setVisible(polygons->at(i).getVisibility());
			graphicsPolygons << p;
		} else {
			QPainterPath path = QPainterPath();
			path.addPolygon(polygons->at(i));
			QGraphicsPathItem* p = this->addPath(path);

			QPen pen(polygons->at(i).getColor());
			pen.setWidth(1);
			p->setPen(pen);
			p->setVisible(polygons->at(i).getVisibility());
			graphicsPolygons << p;
		}

		QList<QGraphicsEllipseItem*> e;
		for(int j=0; j<polygons->at(i).size(); j++) {
			QRectF rect(polygons->at(i).at(j) - QPointF(2, 2), QSize(4, 4));
			e << this->addEllipse(rect);

			e[j]->setPen(QPen(Qt::transparent));
			e[j]->setBrush(QBrush(polygons->at(i).getColor()));
			if(i == polygons->getCurrentIndex())
				if(j == polygons->at(i).getCurrentPointIdx())
					e[j]->setBrush(QBrush(Qt::red));

			e[j]->setVisible(polygons->at(i).getVisibility());
			if(polygons->at(i).getVisibility()) {
				e[j]->setCursor(Qt::OpenHandCursor);

				if(i == polygons->getCurrentIndex() && j == polygons->getCurrentPolygonPointIndex() && pointGrabbed)
					e[j]->setCursor(Qt::ClosedHandCursor);
			}
		}
		graphicsPolygonsPoint << e;
	}
}


void Scene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(mode == Sukyan::CURSOR)
        return;

	if(mode == Sukyan::CONTRAST) {
		firstClick = event->screenPos();
		buttonPressed = event->button();
		return;
	}

	pointGrabbed = false;
	if(mode == Sukyan::POLYGON) {
		if(polygons->clickOnPoint(event->scenePos(), 5)) {
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

	if(mode == Sukyan::CONTRAST) {
		if(buttonPressed == Qt::LeftButton || buttonPressed == Qt::RightButton)
			changeContrast(event->screenPos().x() - firstClick.x(), firstClick.y() - event->screenPos().y());

		backgroundGraphicsPixmap->setCursor(Qt::SizeAllCursor);
		return;
	}

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

	if(pointGrabbed) {
		graphicsPolygonsPoint[polygons->getCurrentIndex()][polygons->getCurrentPolygonPointIndex()]->setCursor(Qt::OpenHandCursor);
		pointGrabbed = false;
	}

	if(mode == Sukyan::CONTRAST) {
		int contrast = firstClick.y() - event->screenPos().y();
		int brightness = event->screenPos().x() - firstClick.x();

		if(brightness < -128) brightness = -128;
		else if(brightness > 128) brightness = 128;
		currentBrightness = brightness;

		if(contrast < - 128) contrast = -128;
		else if(contrast > 128) contrast = 128;
		currentContrast = contrast;
		backgroundGraphicsPixmap->setCursor(Qt::ArrowCursor);

		emit contrastChanged(currentBrightness, currentContrast);
	}

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

void Scene::changeContrast(int brightness, int contrast) {
	brightness += currentBrightness;
	if(brightness < -128) brightness = -128;
	else if(brightness > 128) brightness = 128;

	contrast += currentContrast;
	if(contrast < - 128) contrast = -128;
	else if(contrast > 128) contrast = 128;

	auto truncate = [] (double value) {
		if(value < 0) return static_cast<unsigned char>(0);
		else if(value > 255) return static_cast<unsigned char>(255);
		else return static_cast<unsigned char>(value);
	};

	double factor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));

	QImage newImg(backgroundPixmap.toImage());
	int w = backgroundPixmap.width();
	int h = backgroundPixmap.height();

	if(!buffer)
		buffer = new unsigned char[4 * w * h];
	unsigned char* bit = newImg.bits();

	unsigned char r,g,b;
	for(int i = 0; i < h; i++){
		for(int j = 0; j < w; j++){
			int currIdx = 4 * (i * w + j);

			r = truncate(factor * (*(bit + currIdx    ) - 128 + 128 + brightness));
			g = truncate(factor * (*(bit + currIdx + 1) - 128 + 128 + brightness));
			b = truncate(factor * (*(bit + currIdx + 2) - 128 + 128 + brightness));

			buffer[currIdx    ] = r;
			buffer[currIdx + 1] = g;
			buffer[currIdx + 2] = b;
			buffer[currIdx + 3] = 255;
		}
	}

	newImg = QImage(buffer, w, h, QImage::Format_RGB32);
	backgroundGraphicsPixmap->setPixmap(QPixmap::fromImage(newImg));
}
