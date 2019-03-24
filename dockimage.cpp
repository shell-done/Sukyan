#include "dockimage.h"

DockImage::DockImage(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    setLayout(layout);

    cursorGroup = new QGroupBox(tr("Image"), this);
    cursorLayout = new QFormLayout(cursorGroup);
    cursorGroup->setLayout(cursorLayout);

    cursorPosLabel = new QLabel(tr("x:0 y:0"), this);
    cursorPosLabel->setAlignment(Qt::AlignRight);
    cursorPixColorLabel = new QLabel(tr("R:0 G:0 B:0"), this);
    cursorPixColorLabel->setAlignment(Qt::AlignRight);
    cursorLayout->addRow(tr("Cursor position : "), cursorPosLabel);
    cursorLayout->addRow(tr("Pixel color : "), cursorPixColorLabel);

    layout->addWidget(cursorGroup);
    layout->addSpacing(10);


    scaleGroup = new QGroupBox(tr("Scale"), this);
    scaleLayout = new QHBoxLayout(scaleGroup);
    scaleGroup->setLayout(scaleLayout);

    scalePxSpin = new QSpinBox(this);
    scalePxSpin->setRange(1, 1000);
    scalePxSpin->setValue(10);
    scalePxSpin->setSuffix("px");
    scaleEqLabel = new QLabel("<=>", this);
    scaleEqLabel->setAlignment(Qt::AlignCenter);
    scaleMmSpin = new QSpinBox(this);
    scaleMmSpin->setRange(1, 1000);
    scaleMmSpin->setSuffix("mm");

	connect(scalePxSpin, SIGNAL(valueChanged(int)), this, SLOT(setRatio(int)));
	connect(scaleMmSpin, SIGNAL(valueChanged(int)), this, SLOT(setRatio(int)));

    scaleLayout->addWidget(scalePxSpin);
    scaleLayout->addWidget(scaleEqLabel);
    scaleLayout->addWidget(scaleMmSpin);

    layout->addWidget(scaleGroup);
    layout->addSpacing(10);


    displayGroup = new QGroupBox(tr("Display"));
    displayLayout = new QGridLayout(displayGroup);
    displayGroup->setLayout(displayLayout);

    displayRotationLayout = new QHBoxLayout();
    displayRotationLabel = new QLabel(tr("Rotation :"), this);
    displayRotationSlider = new QSlider(Qt::Horizontal, this);
    displayRotationSlider->setRange(-180, 180);
    displayRotationSlider->setValue(0);
    displayRotationSpin = new QSpinBox(this);
    displayRotationSpin->setRange(-180, 180);
    displayRotationSpin->setValue(0);
    displayRotationSpin->setSuffix(tr("°"));
    displayRotationLayout->addWidget(displayRotationSlider);
    displayRotationLayout->addWidget(displayRotationSpin);

    displayZoomLayout = new QHBoxLayout();
    displayZoomLabel = new QLabel(tr("Zoom :"), this);
    displayZoomSlider = new QSlider(Qt::Horizontal, this);
	displayZoomSlider->setRange(25, 1000);
    displayZoomSlider->setValue(100);
    displayZoomSpin = new QSpinBox(this);
	displayZoomSpin->setRange(25, 1000);
    displayZoomSpin->setValue(100);
    displayZoomSpin->setSuffix("%");
    displayZoomLayout->addWidget(displayZoomSlider);
    displayZoomLayout->addWidget(displayZoomSpin);

    displayLayout->addWidget(displayRotationLabel, 0, 0);
    displayLayout->addLayout(displayRotationLayout, 1, 0);
    displayLayout->addWidget(displayZoomLabel, 2, 0);
    displayLayout->addLayout(displayZoomLayout, 3, 0);

    connect(displayRotationSlider, SIGNAL(valueChanged(int)), this, SLOT(rotationValueChanged(int)));
    connect(displayRotationSpin, SIGNAL(valueChanged(int)), this, SLOT(rotationValueChanged(int)));
    connect(displayZoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomValueChanged(int)));
    connect(displayZoomSpin, SIGNAL(valueChanged(int)), this, SLOT(zoomValueChanged(int)));

    layout->addWidget(displayGroup);

	setFixedWidth(300);
    setMaximumHeight(310);
}

DockImage::~DockImage() {
    delete displayRotationLayout;
    delete displayZoomLayout;
}

void DockImage::cursorInformationChanged(Sukyan::cursorInformation cursorInfo) {
    cursorPosLabel->setText(tr("x:%1 y:%2").arg(cursorInfo.position.x()).arg(cursorInfo.position.y()));
    cursorPixColorLabel->setText(tr("R:%1 G:%2 B:%3").arg(cursorInfo.pixelColor.red(), 3, 10, QChar('0')).arg(cursorInfo.pixelColor.green(), 3, 10, QChar('0')).arg(cursorInfo.pixelColor.blue(), 3, 10, QChar('0')));
}

void DockImage::rotationValueChanged(int v) {
    displayRotationSlider->blockSignals(true);
    displayRotationSpin->blockSignals(true);
    displayRotationSlider->setValue(v);
    displayRotationSpin->setValue(v);
    displayRotationSlider->blockSignals(false);
    displayRotationSpin->blockSignals(false);

    emit rotationChanged(v);
}

void DockImage::zoomValueChanged(int v) {
    displayZoomSlider->blockSignals(true);
    displayZoomSpin->blockSignals(true);
    displayZoomSlider->setValue(v);
    displayZoomSpin->setValue(v);
    displayZoomSlider->blockSignals(false);
    displayZoomSpin->blockSignals(false);

    emit zoomChanged(v);
}

void DockImage::setRatio(int i) {
	Q_UNUSED(i);

	double ratio = scaleMmSpin->value()*1./scalePxSpin->value();
	emit ratioChanged(ratio);
}
