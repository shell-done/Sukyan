#include "dockselection.h"

DockSelection::DockSelection(QWidget *parent) : QWidget(parent) {
	layout = new QVBoxLayout(this);
	setLayout(layout);

	fileGroup = new QGroupBox(tr("Files"), this);
	fileLayout = new QHBoxLayout();
	fileImport = new QPushButton(tr("Import"), this);
	fileExport = new QPushButton(tr("Export"), this);
	fileLayout->addWidget(fileImport);
	fileLayout->addWidget(fileExport);

	connect(fileImport, &QPushButton::clicked, [this](){currentPolygonList->importCSV();});
	connect(fileExport, &QPushButton::clicked, [this](){currentPolygonList->exportCSV();});

	fileGroup->setLayout(fileLayout);
	layout->addWidget(fileGroup);


	polygonsGroup = new QGroupBox(tr("Polygons"), this);
	polygonsLayout = new QVBoxLayout();
	polygonsList = new QComboBox(this);
	polygonsList->setEditable(true);
	polygonsList->setInsertPolicy(QComboBox::InsertAtCurrent);
	polygonsList->addItem("Polygon 1");
	polygonsLayout->addWidget(polygonsList);
	newPolygonButton = new QPushButton(tr("New"), this);
	delPolygonButton = new QPushButton(tr("Delete"), this);

	connect(polygonsList, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
	connect(polygonsList, SIGNAL(editTextChanged(const QString&)), this, SLOT(setCurrentPolygonName(const QString&)));
	connect(newPolygonButton, SIGNAL(clicked()), this, SLOT(newPolygon()));
	connect(delPolygonButton, SIGNAL(clicked()), this, SLOT(delPolygon()));

	QHBoxLayout* ndLayout = new QHBoxLayout();
	ndLayout->addWidget(newPolygonButton);
	ndLayout->addWidget(delPolygonButton);
	polygonsLayout->addLayout(ndLayout);

	QHBoxLayout* visibLayout = new QHBoxLayout();
	QLabel* label = new QLabel(tr("Show :"), this);
	visibility = new QCheckBox(this);
	visibility->setLayoutDirection(Qt::RightToLeft);
	visibility->setChecked(true);
	visibLayout->addWidget(label);
	visibLayout->addWidget(visibility);

	connect(visibility, SIGNAL(stateChanged(int)), this, SLOT(setPolygonVisibility(int)));
	polygonsLayout->addLayout(visibLayout);

	QHBoxLayout* cshapeLayout = new QHBoxLayout();
	label = new QLabel(tr("Closed shape :"), this);
	closedShape = new QCheckBox(this);
	closedShape->setLayoutDirection(Qt::RightToLeft);
	closedShape->setChecked(true);
	cshapeLayout->addWidget(label);
	cshapeLayout->addWidget(closedShape);

	connect(closedShape, SIGNAL(stateChanged(int)), this, SLOT(setPolygonClosedShape(int)));
	polygonsLayout->addLayout(cshapeLayout);

	QHBoxLayout* colorLayout = new QHBoxLayout();
	label = new QLabel(tr("Pick color :"));
	colorPicker = new QToolButton(this);
	setColor(Qt::yellow);
	colorLayout->addWidget(label);
	colorLayout->addWidget(colorPicker);
	connect(colorPicker, SIGNAL(clicked()), this, SLOT(setPolygonColor()));

	polygonsLayout->addLayout(colorLayout);


	pointsGroup = new QGroupBox(tr("Points"), this);
	pointsLayout = new QGridLayout();
	pointsList = new QListWidget();
	pointsLayout->addWidget(pointsList, 0, 0, 2, 1);
	pointsGroup->setDisabled(true);

	connect(pointsList, SIGNAL(currentRowChanged(int)), this, SLOT(setPointListIndex(int)));

	polygonsGroup->setLayout(polygonsLayout);
	layout->addWidget(polygonsGroup);

	QFormLayout* coordsLayout = new QFormLayout();
	pointX = new QSpinBox(this);
	pointX->setRange(0, 1920);
	pointX->setSuffix("px");
	pointY = new QSpinBox(this);
	pointY->setRange(0, 1920);
	pointY->setSuffix("px");
	coordsLayout->addRow("x: ", pointX);
	coordsLayout->addRow("y: ", pointY);
	pointsLayout->addLayout(coordsLayout, 0, 1);

	connect(pointX, SIGNAL(valueChanged(int)), this, SLOT(changePoint(int)));
	connect(pointY, SIGNAL(valueChanged(int)), this, SLOT(changePoint(int)));

	delPointButton = new QPushButton(tr("Delete point"), this);
	pointsLayout->addWidget(delPointButton, 1, 1);
	connect(delPointButton, SIGNAL(clicked()), this, SLOT(delPoint()));

	pointsGroup->setLayout(pointsLayout);
	layout->addWidget(pointsGroup);

	surfaceGroup = new QGroupBox(tr("Surface"), this);
	surfaceLayout = new QHBoxLayout(surfaceGroup);
	surfaceGroup->setLayout(surfaceLayout);
	surfacePx = new QLabel(this);
	surfaceMm = new QLabel(this);

	surfacePx->setText("0 px²");
	surfacePx->setAlignment(Qt::AlignCenter);
	surfaceMm->setText("0 mm²");
	surfaceMm->setAlignment(Qt::AlignCenter);

	surfaceLayout->addWidget(surfacePx);
	surfaceLayout->addWidget(surfaceMm);

	layout->addWidget(surfaceGroup);

	setFixedWidth(300);
	setMaximumHeight(500);

	currentPolygonList = nullptr;
	surfaceRatio = 0.1;
}

void DockSelection::disconnectCurrentPolygonList() {
	if(currentPolygonList)
		disconnect(currentPolygonList, SIGNAL(updtDock()), this, SLOT(updateDock()));

	currentPolygonList = nullptr;
}

void DockSelection::setCurrentPolygonList(PolygonsList* polygonList) {
	if(currentPolygonList)
		disconnect(currentPolygonList, SIGNAL(updtDock()), this, SLOT(updateDock()));

	currentPolygonList = polygonList;
	connect(currentPolygonList, SIGNAL(updtDock()), this, SLOT(updateDock()));

	updateDock();
	currentPolygonList->updateScene();
}

void DockSelection::setColor(QColor c) {
	QPixmap colorPickerPixmap(100, 100);
	colorPickerPixmap.fill(Qt::transparent);

	QPainter painter(&colorPickerPixmap);
	painter.setBrush(QBrush(c));
	painter.drawRect(10, 10, 80, 80);
	painter.end();

	colorPicker->setIcon(QIcon(colorPickerPixmap));
}

void DockSelection::setSurfaceRatio(double r) {
	surfaceRatio = r;
	updateDock();
}

void DockSelection::indexChanged(int idx) {
	if(currentPolygonList->getCurrentIndex() == -1)
		return;

	currentPolygonList->setCurrentIndex(idx);

	updateDock();

	currentPolygonList->updateScene();
}

void DockSelection::newPolygon() {
	currentPolygonList->newPolygon();
	delPolygonButton->setEnabled(true);
	visibility->setEnabled(true);
	colorPicker->setEnabled(true);
}

void DockSelection::delPolygon() {
	currentPolygonList->delPolygon();
	if(currentPolygonList->size() == 0) {
		delPolygonButton->setDisabled(true);
		visibility->setDisabled(true);
		colorPicker->setDisabled(true);
	}
}

void DockSelection::setCurrentPolygonName(const QString& str) {
	if(currentPolygonList->getPolygonsName().contains(str))
		return;

	if(currentPolygonList->getCurrentIndex() == -1)
		return;

	currentPolygonList->setCurrentPolygonName(str);
}


void DockSelection::setPolygonVisibility(int state) {
	if(state)
		currentPolygonList->setCurrentPolygonVisible(true);
	else
		currentPolygonList->setCurrentPolygonVisible(false);
}

void DockSelection::setPolygonColor() {
	QColor c = QColorDialog::getColor();

	setColor(c);
	if(currentPolygonList)
		currentPolygonList->setCurrentPolygonColor(c);
}

void DockSelection::setPolygonClosedShape(int state) {
	if(state)
		currentPolygonList->setCurrentPolygonClosedShape(true);
	else
		currentPolygonList->setCurrentPolygonClosedShape(false);

	updateDock();
}

void DockSelection::setPointListIndex(int idx) {
	pointX->blockSignals(true);
	pointY->blockSignals(true);

	currentPolygonList->setCurrentPolygonPointIndex(idx);
	QPoint p = currentPolygonList->getCurrentPolygonPoint();
	pointX->setValue(p.x());
	pointY->setValue(p.y());

	pointX->blockSignals(false);
	pointY->blockSignals(false);
}

void DockSelection::delPoint() {
	currentPolygonList->removePointToCurrentPolygon();
}

void DockSelection::changePoint(int u) {
	Q_UNUSED(u);
	pointX->blockSignals(true);
	pointY->blockSignals(true);

	QPoint p(pointX->value(), pointY->value());
	currentPolygonList->setCurrentPolygonPoint(p);

	pointX->blockSignals(false);
	pointY->blockSignals(false);
}


void DockSelection::updateDock() {
	polygonsList->blockSignals(true);
	pointsList->blockSignals(true);
	pointX->blockSignals(true);
	pointY->blockSignals(true);

	polygonsList->clear();
	polygonsList->addItems(currentPolygonList->getPolygonsName());
	polygonsList->setCurrentIndex(currentPolygonList->getCurrentIndex());
	pointsList->clear();

	if(currentPolygonList->getCurrentIndex() != -1) {
		visibility->setChecked(currentPolygonList->getCurrentPolygonVisibility());
		closedShape->setChecked(currentPolygonList->getCurrentPolygonClosedShape());
		setColor(currentPolygonList->getCurrentPolygonColor());

		QStringList pList = currentPolygonList->getCurrentPolygonPointsStr();
		if(pList.size()) {
			pointsList->addItems(pList);
			pointsList->setCurrentRow(currentPolygonList->getCurrentPolygonPointIndex());
			QPoint p = currentPolygonList->getCurrentPolygonPoint();
			pointX->setValue(p.x());
			pointY->setValue(p.y());

			pointsGroup->setEnabled(true);
		} else {
			pointsGroup->setDisabled(true);
		}

		if(pList.size() == 2 || !currentPolygonList->getCurrentPolygonClosedShape()) {
			double distance = currentPolygonList->getCurrentPolygonSurface();

			surfaceGroup->setEnabled(true);
			surfaceGroup->setTitle(tr("Distance"));
			surfacePx->setText(QString("%1 px").arg(distance));
			surfaceMm->setText(QString("%1 mm").arg(distance*surfaceRatio));
		} else if(pList.size() > 2) {
			double surface = currentPolygonList->getCurrentPolygonSurface();

			surfaceGroup->setEnabled(true);
			surfaceGroup->setTitle(tr("Surface"));
			surfacePx->setText(QString("%1 px²").arg(surface));
			surfaceMm->setText(QString("%1 mm²").arg(surface*surfaceRatio*surfaceRatio));
		} else {
			surfaceGroup->setDisabled(true);
			surfaceGroup->setTitle(tr("Distance"));
			surfacePx->setText("0 px");
			surfaceMm->setText("0 mm");
		}

	} else {
		pointsGroup->setDisabled(true);
		surfaceGroup->setDisabled(true);
		surfaceGroup->setTitle(tr("Distance"));
		surfacePx->setText("0 px");
		surfaceMm->setText("0 mm");
	}

	bool pointsAvailable = false;
	for(int i=0; i<currentPolygonList->size(); i++)
		if(currentPolygonList->at(i).size())
			pointsAvailable = true;

	if(pointsAvailable) {
		fileExport->setEnabled(true);
	} else {
		fileExport->setDisabled(true);
	}

	pointX->blockSignals(false);
	pointY->blockSignals(false);
	polygonsList->blockSignals(false);
	pointsList->blockSignals(false);
}
