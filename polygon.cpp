#include "polygon.h"

Polygon::Polygon(QString n) : QPolygon(), name(n) {
	color = QColor(0, 0, 0);
	visible = true;
	currentPoint = -1;
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

void Polygon::setCurrentPointIdx(int idx) {
	currentPoint = idx;
}

void Polygon::setCurrentPoint(QPoint p) {
	replace(currentPoint, p);
}

bool Polygon::getVisibility() {return visible;}

QColor Polygon::getColor() {return color;}

int Polygon::getCurrentPointIdx() {return currentPoint;}

QPoint Polygon::getCurrentPoint() {return at(currentPoint);}

double Polygon::surface() {
	if(count() < 2)
		return 0;

	if(count() == 2)
		return sqrt(pow(point(0).x() - point(1).x(), 2) + pow(point(0).y() - point(1).y(), 2));

	int A = 0, B = 0;

	for(int i=0; i<count(); i++) {
		A += point(i).x() * point((i+1)%count()).y();
		B += point(i).y() * point((i+1)%count()).x();
	}

	return abs(A-B)/2.;
}
