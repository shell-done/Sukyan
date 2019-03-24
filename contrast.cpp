#include "contrast.h"

ContrastDialog::ContrastDialog(QWidget *parent) : QDialog(parent) {
	setWindowTitle("Brightness / Contrast");
	setModal(true);
	hide();
	layout = new QVBoxLayout(this);
	setLayout(layout);
	setFixedSize(300, 110);

	QLabel* brightnessLabel = new QLabel(tr("Brightness :"), this);
	layout->addWidget(brightnessLabel);

	brightness = new QSlider(Qt::Horizontal, this);
	brightness->setRange(-128, 128);
	connect(brightness, SIGNAL(valueChanged(int)), this, SLOT(changeBrightness(int)));
	brightnessValue = new QSpinBox(this);
	brightnessValue->setRange(-128, 128);
	connect(brightnessValue, SIGNAL(valueChanged(int)), this, SLOT(changeBrightness(int)));
	brightnessReset = new QToolButton(this);
	brightnessReset->setIcon(QIcon(":/icons/reset.png"));
	connect(brightnessReset, SIGNAL(clicked()), this, SLOT(resetBrightness()));

	QHBoxLayout* brightnessLayout = new QHBoxLayout();
	brightnessLayout->addWidget(brightness);
	brightnessLayout->addWidget(brightnessValue);
	brightnessLayout->addWidget(brightnessReset);
	layout->addLayout(brightnessLayout);

	QLabel* contrastLabel = new QLabel(tr("Contrast :"), this);
	layout->addWidget(contrastLabel);

	contrast = new QSlider(Qt::Horizontal, this);
	contrast->setRange(-255, 255);
	connect(contrast, SIGNAL(valueChanged(int)), this, SLOT(changeContrast(int)));
	contrastValue = new QSpinBox(this);
	contrastValue->setRange(-128, 128);
	connect(contrastValue, SIGNAL(valueChanged(int)), this, SLOT(changeContrast(int)));
	contrastReset = new QToolButton(this);
	contrastReset->setIcon(QIcon(":/icons/reset.png"));
	connect(contrastReset, SIGNAL(clicked()), this, SLOT(resetContrast()));

	QHBoxLayout* contrastLayout = new QHBoxLayout();
	contrastLayout->addWidget(contrast);
	contrastLayout->addWidget(contrastValue);
	contrastLayout->addWidget(contrastReset);
	layout->addLayout(contrastLayout);
}

void ContrastDialog::closeEvent(QCloseEvent *event) {
	Q_UNUSED(event);
	hide();
}

void ContrastDialog::setBrightness(int b) {
	brightness->blockSignals(true);
	brightnessValue->blockSignals(true);
	brightness->setValue(b);
	brightnessValue->setValue(b);
	brightness->blockSignals(false);
	brightnessValue->blockSignals(false);
}

void ContrastDialog::setContrast(int c) {
	contrast->blockSignals(true);
	contrastValue->blockSignals(true);
	contrast->setValue(c);
	contrastValue->setValue(c);
	contrast->blockSignals(false);
	contrastValue->blockSignals(false);
}

void ContrastDialog::changeBrightness(int b) {
	setBrightness(b);
	emit contrastChanged(b, contrast->value());
}

void ContrastDialog::setValues(int b, int c) {
	setBrightness(b);
	setContrast(c);
}

void ContrastDialog::changeContrast(int c) {
	setContrast(c);
	emit contrastChanged(brightness->value(), c);
}

void ContrastDialog::resetBrightness() {
	setBrightness(0);
	emit contrastChanged(0, contrast->value());
}

void ContrastDialog::resetContrast() {
	setContrast(0);
	emit contrastChanged(brightness->value(), 0);
}
