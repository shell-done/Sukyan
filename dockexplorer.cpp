#include "dockexplorer.h"

DockExplorer::DockExplorer(QWidget *parent) : QListWidget(parent) {
    setViewMode(QListWidget::IconMode);
    setIconSize(QSize(200, 150));
    setResizeMode(QListWidget::Adjust);
    setFlow(QListView::TopToBottom);
    setMovement(QListView::Static);

    setStyleSheet("QListView::item {margin-top:15px;}");
	setContextMenuPolicy(Qt::CustomContextMenu);

    setFixedWidth(210);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

void DockExplorer::addThumbnail(QPixmap* pixmap, QString fileName) {
    QString thumbTitle;
	if(fileName.size() > 32)
		thumbTitle = fileName.left(30) + "...";
    else
        thumbTitle = fileName;

    QListWidgetItem* item = new QListWidgetItem(QIcon(*pixmap), thumbTitle, this);
    thumbnails.append(item);

    addItem(item);
    blockSignals(true);
    setCurrentItem(item);
    blockSignals(false);
}

void DockExplorer::removeThumbnail(int idx) {
	QListWidgetItem* item = thumbnails.takeAt(idx);
	removeItemWidget(item);
	delete item;
}

void DockExplorer::setCurrentThumbnail(int idx) {
	blockSignals(true);
	setCurrentRow(idx);
	blockSignals(false);
}

void DockExplorer::showContextMenu(QPoint pos) {
	QPoint globalPos = mapToGlobal(pos);

	QMenu menu;
	menu.addAction(tr("Save"), [this](){saveImageClicked();});
	menu.addAction(tr("Close"), [this](){closeImageClicked();});
	if(thumbnails.size() == 1)
		menu.actions()[1]->setDisabled(true);

	menu.exec(globalPos);
}

void DockExplorer::saveImageClicked() {
	emit saveImage(currentRow());
}

void DockExplorer::closeImageClicked() {
	emit closeImage(currentRow());
}
