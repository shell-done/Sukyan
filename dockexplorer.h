#ifndef DOCKEXPLORER_H
#define DOCKEXPLORER_H

#include <QListWidget>
#include "core.h"

class DockExplorer : public QListWidget {
    Q_OBJECT

public:
    explicit DockExplorer(QWidget *parent = nullptr);

    void addThumbnail(QPixmap* pixmap, QString fileName);
	void removeThumbnail(int);
	void setCurrentThumbnail(int);

private:
    QList<QListWidgetItem*> thumbnails;

private slots:
	void showContextMenu(QPoint p);
	void saveImageClicked();
	void closeImageClicked();

signals:
	void saveImage(int);
	void closeImage(int);
};

#endif // DOCKEXPLORER_H
