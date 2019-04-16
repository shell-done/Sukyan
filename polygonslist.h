#ifndef POLYGONSLIST_H
#define POLYGONSLIST_H

#include <QObject>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStringRef>
#include "polygon.h"

class PolygonsList : public QObject {
	Q_OBJECT

public:
	PolygonsList();
	PolygonsList(const PolygonsList &oldPoly);
	~PolygonsList();

	void setCurrentIndex(int);
	int getCurrentIndex();

	class Polygon at(int) const;
	int size() const;
	QStringList getPolygonsName();
	bool nameExists(QString);
	bool clickOnPoint(QPointF, int);
	bool saved();

	void newPolygon();
	void newPolygon(const class Polygon oldPoly);
	void delPolygon();
	void setCurrentPolygonName(QString str);
	void setCurrentPolygonVisible(bool);
	void setCurrentPolygonColor(QColor);
	void setCurrentPolygonClosedShape(bool);
	void setCurrentPolygonPointIndex(int);
	void setCurrentPolygonPoint(QPoint);

	QString getCurrentPolygonName();
	bool getCurrentPolygonVisibility();
	QColor getCurrentPolygonColor();
	bool getCurrentPolygonClosedShape();
	QVector<QPoint> getCurrentPolygonPoints();
	QStringList getCurrentPolygonPointsStr();
	int getCurrentPolygonPointIndex();
	QPoint getCurrentPolygonPoint();
	double getCurrentPolygonSurface();

	void appendPointToCurrentPolygon(QPoint p);
	void removePointToCurrentPolygon();

	void updateDock();
	void updateScene();

	bool exportCSV(QWidget* parent = nullptr);
	void importCSV(QWidget* parent = nullptr);

private:
	int currentIndex;
	QList<class Polygon*> polygons;
	bool pointsSaved;

signals:
	void updtDock();
	void updtScene();
};

#endif // POLYGONSLIST_H
