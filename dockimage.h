#ifndef DOCKIMAGE_H
#define DOCKIMAGE_H

#include <QtWidgets>
#include "core.h"

class DockImage : public QWidget {
    Q_OBJECT

public:
    explicit DockImage(QWidget *parent = nullptr);
    ~DockImage();

private:
    QVBoxLayout* layout;

    QGroupBox* cursorGroup;
    QFormLayout* cursorLayout;
    QLabel* cursorPosLabel;
    QLabel* cursorPixColorLabel;

    QGroupBox* scaleGroup;
    QHBoxLayout* scaleLayout;
    QSpinBox* scalePxSpin;
    QLabel* scaleEqLabel;
    QSpinBox* scaleMmSpin;

    QHBoxLayout* displayRotationLayout;
    QHBoxLayout* displayZoomLayout;
    QGroupBox* displayGroup;
    QGridLayout* displayLayout;
    QLabel* displayRotationLabel;
    QSlider* displayRotationSlider;
    QSpinBox* displayRotationSpin;
    QLabel* displayZoomLabel;
    QSlider* displayZoomSlider;
    QSpinBox* displayZoomSpin;


public slots:
    void cursorInformationChanged(Sukyan::cursorInformation);
    void rotationValueChanged(int);
    void zoomValueChanged(int);

private slots:
	void setRatio(int);

signals:
    void rotationChanged(int);
    void zoomChanged(int);
	void ratioChanged(double);
};

#endif // DOCKIMAGE_H
