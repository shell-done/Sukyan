#ifndef DOCKDRAWING_H
#define DOCKDRAWING_H

#include <QWidget>
#include <QtWidgets>

class DockDrawing : public QWidget {
	Q_OBJECT
public:
	explicit DockDrawing(QWidget *parent = nullptr);
	void setColor(QColor);

private:
	void initWidgets();

	QCheckBox* displayedCB;
	QToolButton* colorPicker;
	QSpinBox* penWidth;
	QPushButton* clearButton;

private slots:
	void penWidthValueChanged(int);
	void CBStateChanged(int);

signals:
	void colorChanged(QColor);
	void penWidthChanged(int);
	void displayChanged(bool);
	void clear();
};

#endif // DOCKDRAWING_H
