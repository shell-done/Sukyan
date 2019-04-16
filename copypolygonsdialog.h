#ifndef COPYPOLYGONSDIALOG_H
#define COPYPOLYGONSDIALOG_H

#include "core.h"
#include "polygonslist.h"

class CopyPolygonsDialog : public QDialog {
	Q_OBJECT
public:
	CopyPolygonsDialog(QWidget* parent);
	void openDialog(PolygonsList* polyList, QStringList img);

protected:
	void closeEvent(QCloseEvent* event);

private:
	QListWidget* list;
	QComboBox* copyDest;

private slots:
	void copyClicked();
	void closeClicked();

signals:
	void copy(QString, QStringList);
	void close();
};

#endif // COPYPOLYGONSDIALOG_H
