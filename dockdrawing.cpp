#include "dockdrawing.h"

DockDrawing::DockDrawing(QWidget *parent) : QWidget(parent) {
	initWidgets();

	setMinimumWidth(250);
	setMaximumWidth(300);
	setMaximumHeight(150);
}

void DockDrawing::initWidgets() {
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	setLayout(mainLayout);
	QGroupBox* drawingGroup = new QGroupBox(tr("Drawing"), this);
	mainLayout->addWidget(drawingGroup);

	QVBoxLayout* layout = new QVBoxLayout(drawingGroup);
	drawingGroup->setLayout(layout);

	QHBoxLayout* dispLayout = new QHBoxLayout();
	QLabel* label = new QLabel(tr("Show drawing :"), this);
	displayedCB = new QCheckBox(this);
	displayedCB->setLayoutDirection(Qt::RightToLeft);
	displayedCB->setChecked(true);
	connect(displayedCB, SIGNAL(stateChanged(int)), this, SLOT(CBStateChanged(int)));

	dispLayout->addWidget(label);
	dispLayout->addWidget(displayedCB);

	QHBoxLayout* colorLayout = new QHBoxLayout();
	label = new QLabel(tr("Pick color :"));
	colorPicker = new QToolButton(this);
	colorLayout->addWidget(label);
	colorLayout->addWidget(colorPicker);
	setColor(Qt::black);
	connect(colorPicker, &QToolButton::clicked, [this](){
		QColor c = QColorDialog::getColor();
		setColor(c);
		emit colorChanged(c);
	});

	QHBoxLayout* pWidthLayout = new QHBoxLayout();
	label = new QLabel(tr("Pen width :"), this);
	penWidth = new QSpinBox(this);
	penWidth->setRange(1, 500);
	penWidth->setValue(10);
	penWidth->setSuffix("px");
	penWidth->setMaximumWidth(80);
	pWidthLayout->addWidget(label);
	pWidthLayout->addWidget(penWidth);
	connect(penWidth, SIGNAL(valueChanged(int)), this, SLOT(penWidthValueChanged(int)));

	clearButton = new QPushButton(tr("Clear drawing"), this);
	connect(clearButton, &QPushButton::clicked, [this](){
		QMessageBox::StandardButton rep;
		rep = QMessageBox::question(this, tr("Clear your drawing"), tr("Are you sure you want to clear your drawing ?"));
		if(rep == QMessageBox::Yes)
			emit clear();
	});

	layout->addLayout(dispLayout);
	layout->addLayout(colorLayout);
	layout->addLayout(pWidthLayout);
	layout->addWidget(clearButton);
}

void DockDrawing::setColor(QColor c) {
	QPixmap colorPickerPixmap(100, 100);
	colorPickerPixmap.fill(Qt::transparent);

	QPainter painter(&colorPickerPixmap);
	painter.setBrush(QBrush(c));
	painter.drawRect(10, 10, 80, 80);
	painter.end();

	colorPicker->setIcon(QIcon(colorPickerPixmap));
}

void DockDrawing::penWidthValueChanged(int v) {
	emit penWidthChanged(v);
}

void DockDrawing::CBStateChanged(int s) {
	bool b = static_cast<bool>(s);
	emit displayChanged(b);
}
