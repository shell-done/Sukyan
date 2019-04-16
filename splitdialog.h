#ifndef SPLITDIALOG_H
#define SPLITDIALOG_H

#include <QVector2D>
#include "core.h"
#include "polygonslist.h"

class SplitDialog : public QDialog {
	Q_OBJECT

public:
	SplitDialog(QWidget* parent);
	void openDialog(PolygonsList* polyList);

protected:
	void closeEvent(QCloseEvent*);

private:
	PolygonsList* polyList;

	QComboBox* polygons;
	QSpinBox* parts;

private slots:
	void split();
};

#endif // SPLITDIALOG_H
