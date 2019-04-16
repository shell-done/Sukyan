#ifndef SELECTION_H
#define SELECTION_H

#include <QtWidgets>
#include "core.h"
#include "polygon.h"
#include "polygonslist.h"

class DockSelection : public QWidget {
    Q_OBJECT

public:
    explicit DockSelection(QWidget *parent = nullptr);
	void disconnectCurrentPolygonList();
	void setCurrentPolygonList(PolygonsList*);

private:
	QVBoxLayout* layout;

	QGroupBox* fileGroup;
	QHBoxLayout* fileLayout;
	QPushButton* fileImport;
	QPushButton* fileExport;

	QGroupBox* polygonsGroup;
	QVBoxLayout* polygonsLayout;
	QComboBox* polygonsList;
	QPushButton* newPolygonButton;
	QPushButton* delPolygonButton;
	QCheckBox* visibility;
	QCheckBox* closedShape;
	QToolButton* colorPicker;

	QGroupBox* pointsGroup;
	QGridLayout* pointsLayout;
    QListWidget* pointsList;
	QSpinBox* pointX;
	QSpinBox* pointY;
	QPushButton* delPointButton;

	QGroupBox* surfaceGroup;
	QHBoxLayout* surfaceLayout;
	QLabel* surfacePx;
	QLabel* surfaceMm;

	PolygonsList* currentPolygonList;
	double surfaceRatio;

	void setColor(QColor c);

public slots:
	void setSurfaceRatio(double);

private slots:
	void indexChanged(int);
	void newPolygon();
	void delPolygon();
	void setCurrentPolygonName(const QString &text);
	void setPolygonVisibility(int state);
	void setPolygonColor();
	void setPolygonClosedShape(int state);
	void setPointListIndex(int);
	void delPoint();
	void changePoint(int);

	void updateDock();
};

#endif // SELECTION_H
