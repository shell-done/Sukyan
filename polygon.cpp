#include "polygon.h"

Polygon::Polygon(QString n) : QPolygon(), name(n) {
	color.setHsv(qrand()%360, qrand()%40 + 215, qrand()%40 + 215);

	visible = true;
	currentPoint = -1;
	closedShape = true;
}

QString Polygon::getName() const {
	return name;
}

void Polygon::setName(QString n) {
	name = n;
}

void Polygon::setVisible(bool v) {
	visible = v;
}

void Polygon::setColor(QColor c) {
	color = c;
}

void Polygon::setClosedShape(bool b) {
	closedShape = b;
}

void Polygon::setCurrentPointIdx(int idx) {
	currentPoint = idx;
}

void Polygon::setCurrentPoint(QPoint p) {
	replace(currentPoint, p);
}

bool Polygon::getVisibility() {return visible;}

QColor Polygon::getColor() {return color;}

bool Polygon::getClosedShape() {
	return closedShape;
}

int Polygon::getCurrentPointIdx() {return currentPoint;}

QPoint Polygon::getCurrentPoint() {return at(currentPoint);}

double Polygon::surface() {
	if(count() < 2)
		return 0;

	if(count() == 2)
		return sqrt(pow(point(0).x() - point(1).x(), 2) + pow(point(0).y() - point(1).y(), 2));

	double surf = 0;
	if(closedShape) {
		int A = 0, B = 0;
		for(int i=0; i<count(); i++) {
			A += point(i).x() * point((i+1)%count()).y();
			B += point(i).y() * point((i+1)%count()).x();
		}

		surf = abs(A-B)/2.;
	} else {
		for(int i=0; i<size() - 1; i++)
			surf += sqrt(pow(point(i).x() - point(i+1).x(), 2) + pow(point(i).y() - point(i+1).y(), 2));
	}

	return surf;
}
