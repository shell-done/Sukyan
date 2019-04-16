#include "splitdialog.h"

SplitDialog::SplitDialog(QWidget* parent) : QDialog (parent) {
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	setWindowTitle(tr("Sample Polygon"));
	setModal(true);
	setFixedSize(300, 90);
	hide();

	QHBoxLayout* splitLayout = new QHBoxLayout();
	QLabel* label = new QLabel(tr("Sample "), this);
	//label->setAlignment(Qt::AlignRight);
	splitLayout->addWidget(label);

	polygons = new QComboBox(this);
	polygons->setFixedWidth(100);
	splitLayout->addWidget(polygons);

	label = new QLabel(tr(" into "), this);
	label->setAlignment(Qt::AlignCenter);
	splitLayout->addWidget(label);

	parts = new QSpinBox(this);
	parts->setRange(1, 500);
	splitLayout->addWidget(parts);

	label = new QLabel(tr(" lines"), this);
	splitLayout->addWidget(label);

	mainLayout->addLayout(splitLayout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	QPushButton* splitButton = new QPushButton(tr("Sample"), this);
	QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);

	buttonsLayout->addSpacing(50);
	buttonsLayout->addWidget(splitButton);
	buttonsLayout->addWidget(cancelButton);

	connect(splitButton, SIGNAL(clicked()), this, SLOT(split()));
	connect(cancelButton, &QPushButton::clicked, [this]() {hide();});

	mainLayout->addLayout(buttonsLayout);
}

void SplitDialog::openDialog(PolygonsList* pls) {
	polyList = pls;

	polygons->clear();
	QStringList polyNamesList;
	for(int i=0; i<polyList->size(); i++)
		if(polyList->at(i).size() > 1)
			polyNamesList << polyList->at(i).getName();
	polygons->addItems(polyNamesList);

	if(polyNamesList.count() == 0) {
		QMessageBox::information(this, "Sukyan", tr("No polygons have more than 2 points"));
		return;
	}

	show();
}

void SplitDialog::closeEvent(QCloseEvent* event) {
	Q_UNUSED(event);
	hide();
}

void SplitDialog::split() {
	class Polygon* splittedPoly = nullptr;

	for(int i=0; i<polyList->size(); i++)
		if(polyList->at(i).getName() == polygons->currentText()) {
			splittedPoly = new class Polygon(polyList->at(i));
		}

	if(!splittedPoly) {
		hide();
		return;
	}

	QString name = splittedPoly->getName() + tr(" splitted");
	int idx = 1;
	while(polyList->nameExists(name)) {
		if(!polyList->nameExists(name + " (" + QString::number(idx) + ")")) {
			name +=" (" + QString::number(idx) + ")";
		}

		idx++;
	}

	splittedPoly->setName(name);
	splittedPoly->setCurrentPointIdx(0);

	double perimeter = 0;
	for(int i=0; i<splittedPoly->size() - 1; i++)
		perimeter += sqrt(pow(splittedPoly->at(i).x() - splittedPoly->at(i+1).x(), 2) + pow(splittedPoly->at(i).y() - splittedPoly->at(i+1).y(), 2));
	if(splittedPoly->getClosedShape() && splittedPoly->size() != 2)
		perimeter += sqrt(pow(splittedPoly->at(0).x() - splittedPoly->at(splittedPoly->size() - 1).x(), 2) + pow(splittedPoly->at(0).y() - splittedPoly->at(splittedPoly->size() - 1).y(), 2));

	QVector<QPoint> points;
	for(int i=0; i<=parts->value(); i++) {
		double currentPointDist = i*perimeter/parts->value();

		int idx = 0;
		QPoint p1 = splittedPoly->at(idx);
		QPoint p2 = splittedPoly->at(idx + 1);
		double currentPerimDist = sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
		while(currentPerimDist < currentPointDist) {
			idx++;
			p1 = splittedPoly->at(idx);
			p2 = splittedPoly->at((idx+1)%splittedPoly->size());
			currentPerimDist += sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
		}
		currentPerimDist -= sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
		if(currentPerimDist < 0) currentPerimDist = 0;
		if(currentPerimDist > perimeter) currentPerimDist = perimeter;

		double distToP1 = currentPointDist - currentPerimDist;
		QVector2D vect(p2.x() - p1.x(), p2.y() - p1.y());
		vect.normalize();
		vect *= static_cast<float>(distToP1);

		QPoint newPoint(static_cast<int>(p1.x() + vect.x()), static_cast<int>(p1.y() + vect.y()));
		points << newPoint;
	}

	splittedPoly->clear();
	splittedPoly->append(points);

	polyList->newPolygon(*splittedPoly);
	polyList->setCurrentIndex(polyList->size() - 1);
	polyList->updateDock();
	polyList->updateScene();
	delete splittedPoly;
	hide();
}




