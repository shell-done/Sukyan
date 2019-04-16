#include "copypolygonsdialog.h"

CopyPolygonsDialog::CopyPolygonsDialog(QWidget* parent) : QDialog(parent) {
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	setWindowTitle(tr("Copy Polygons"));
	setModal(true);
	hide();
	setFixedSize(250, 300);

	list = new QListWidget();
	mainLayout->addWidget(list);

	QFormLayout* copyLayout = new QFormLayout();
	copyDest = new QComboBox(this);
	copyLayout->addRow(tr("Destination :"), copyDest);

	mainLayout->addLayout(copyLayout);

	QHBoxLayout* buttonsLayout = new QHBoxLayout();
	QPushButton* copy = new QPushButton(tr("Copy"));
	QPushButton* cancel = new QPushButton(tr("Cancel"));
	buttonsLayout->addSpacing(150);
	buttonsLayout->addWidget(copy);
	buttonsLayout->addWidget(cancel);

	connect(copy, SIGNAL(clicked()), this, SLOT(copyClicked()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(closeClicked()));

	mainLayout->addLayout(buttonsLayout);
}

void CopyPolygonsDialog::closeEvent(QCloseEvent *event) {
	Q_UNUSED(event);
	hide();
}

void CopyPolygonsDialog::openDialog(PolygonsList* polyList, QStringList img) {
	while(list->count()) {
		QListWidgetItem* item = list->takeItem(0);
		delete item;
	}

	for(int i=0; i<polyList->size(); i++) {
		QListWidgetItem* item = new QListWidgetItem(polyList->at(i).getName(), list);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(Qt::Checked);
	}

	copyDest->clear();
	copyDest->addItems(img);

	show();
}

void CopyPolygonsDialog::copyClicked() {
	QStringList polyToCopy;
	for(int i=0; i<list->count(); i++)
		if(list->item(i)->checkState() == Qt::Checked)
			polyToCopy << list->item(i)->text();

	emit copy(copyDest->currentText(), polyToCopy);
	hide();
}

void CopyPolygonsDialog::closeClicked() {
	hide();
}
