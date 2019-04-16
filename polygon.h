#ifndef POLYGON_H
#define POLYGON_H

#include <QObject>
#include <QList>
#include <QPolygon>
#include <QPolygonF>
#include <QColor>

class Polygon : public QPolygon {
public:
	Polygon(QString name);

	QString getName() const;

	void setName(QString name);
	void setVisible(bool);
	void setColor(QColor);
	void setClosedShape(bool);
	void setCurrentPointIdx(int);
	void setCurrentPoint(QPoint);

	bool getVisibility();
	QColor getColor();
	bool getClosedShape();
	int getCurrentPointIdx();
	QPoint getCurrentPoint();

	double surface();

private:
	QString name;
	bool visible;
	QColor color;
	bool closedShape;

	int currentPoint;
};

#endif // POLYGON_H
