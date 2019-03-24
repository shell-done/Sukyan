#ifndef CONTRAST_H
#define CONTRAST_H

#include <QWidget>
#include "core.h"

class ContrastDialog : public QDialog {
	Q_OBJECT

public:
	ContrastDialog(QWidget *parent = nullptr);
	void setBrightness(int);
	void setContrast(int);

protected:
	void closeEvent(QCloseEvent* event);

private:
	QVBoxLayout* layout;

	QSlider* brightness;
	QSpinBox* brightnessValue;
	QToolButton* brightnessReset;

	QSlider* contrast;
	QSpinBox* contrastValue;
	QToolButton* contrastReset;

public slots:
	void setValues(int, int);

private slots:
	void changeBrightness(int);
	void changeContrast(int);
	void resetBrightness();
	void resetContrast();

signals:
	void contrastChanged(int, int);
};

#endif // CONTRAST_H
