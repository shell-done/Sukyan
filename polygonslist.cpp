#include "polygonslist.h"

PolygonsList::PolygonsList() {
	Polygon* poly = new Polygon("Polygon 1");
	polygons.append(poly);

	currentIndex = 0;
	pointsSaved = true;
}

PolygonsList::~PolygonsList() {
	for(int i=0; i<polygons.size(); i++)
		delete polygons[i];
}

void PolygonsList::setCurrentIndex(int idx) {currentIndex = idx;}

int PolygonsList::getCurrentIndex() {return currentIndex;}

class Polygon PolygonsList::at(int idx) {return *(polygons.at(idx));}

int PolygonsList::size() {return polygons.size();}

QStringList PolygonsList::getPolygonsName() {
	QStringList list;

	for(int i=0; i<polygons.size(); i++)
		list += polygons[i]->getName();

	return list;
}

bool PolygonsList::nameAlreadyExists(QString str) {
	for(int i=0; i<polygons.size(); i++)
		if(polygons[i]->getName() == str)
			return true;

	return false;
}

bool PolygonsList::clickOnPoint(QPointF p, int dist) {
	for(int i=0; i<polygons.size(); i++) {
		if(!polygons[i]->getVisibility())
			continue;

		for(int j=0; j<polygons[i]->size(); j++) {
			if(pow(p.x() - polygons[i]->at(j).x(), 2) + pow(p.y() - polygons[i]->at(j).y(), 2) <= dist) {
				setCurrentIndex(i);
				setCurrentPolygonPointIndex(j);
				return true;
			}
		}
	}

	return false;
}

bool PolygonsList::saved() {return pointsSaved;}

void PolygonsList::newPolygon() {
	QString name = "Polygon ";
	int idx = 0;
	bool ok = false;

	while(!ok) {
		idx++;

		ok = true;
		for(int i=0; i<polygons.size(); i++) {
			if(polygons[i]->getName() == name + QString::number(idx))
				ok = false;
		}
	}

	Polygon* poly = new Polygon(name + QString::number(idx));
	polygons.append(poly);

	currentIndex = polygons.size() - 1;
	updateDock();
	updateScene();

	pointsSaved = false;
}

void PolygonsList::delPolygon() {
	if(currentIndex < 0 || currentIndex >= polygons.size())
		return;

	Polygon* poly = polygons.takeAt(currentIndex);
	delete poly;

	if(polygons.size() == 0)
		currentIndex = -1;
	else if(currentIndex != 0)
		currentIndex--;

	updateDock();
	updateScene();

	pointsSaved = false;
}

void PolygonsList::setCurrentPolygonName(QString str) {
	polygons[currentIndex]->setName(str);

	pointsSaved = false;
}

void PolygonsList::setCurrentPolygonVisible(bool v) {
	polygons[currentIndex]->setVisible(v);
	updateScene();
}

void PolygonsList::setCurrentPolygonColor(QColor c) {
	polygons[currentIndex]->setColor(c);
	updateScene();
}

void PolygonsList::setCurrentPolygonPointIndex(int idx) {
	polygons[currentIndex]->setCurrentPointIdx(idx);
	updateScene();
}

void PolygonsList::setCurrentPolygonPoint(QPoint p) {
	polygons[currentIndex]->setCurrentPoint(p);

	updateScene();
	updateDock();
	pointsSaved = false;
}

QString PolygonsList::getCurrentPolygonName() {return polygons[currentIndex]->getName();}

bool PolygonsList::getCurrentPolygonVisibility() {return polygons[currentIndex]->getVisibility();}

QColor PolygonsList::getCurrentPolygonColor() {return polygons[currentIndex]->getColor();}

QVector<QPoint> PolygonsList::getCurrentPolygonPoints() {
	QVector<QPoint> vect(*polygons[currentIndex]);
	return vect;
}

QStringList PolygonsList::getCurrentPolygonPointsStr() {
	QStringList list;
	for(int i=0; i<polygons[currentIndex]->size(); i++)
		list += QString("(x: %1; y: %2)").arg(polygons[currentIndex]->at(i).x()).arg(polygons[currentIndex]->at(i).y());

	return list;
}

int PolygonsList::getCurrentPolygonPointIndex() {return polygons[currentIndex]->getCurrentPointIdx();}

QPoint PolygonsList::getCurrentPolygonPoint() {return polygons[currentIndex]->getCurrentPoint();}

double PolygonsList::getCurrentPolygonSurface() {return polygons[currentIndex]->surface();}

void PolygonsList::appendPointToCurrentPolygon(QPoint p) {
	int nextIdx = polygons[currentIndex]->getCurrentPointIdx() + 1;

	polygons[currentIndex]->insert(nextIdx, p);
	polygons[currentIndex]->setCurrentPointIdx(nextIdx);

	pointsSaved = false;
	updateDock();
	updateScene();
}

void PolygonsList::removePointToCurrentPolygon() {
	if(currentIndex == -1)
		return;

	int idx = polygons[currentIndex]->getCurrentPointIdx();
	polygons[currentIndex]->removeAt(idx);

	if(polygons[currentIndex]->size() == 0)
		idx = -1;
	else if(idx != 0)
		idx--;

	polygons[currentIndex]->setCurrentPointIdx(idx);

	pointsSaved = false;
	updateDock();
	updateScene();
}

void PolygonsList::updateDock() {emit updtDock();}
void PolygonsList::updateScene() {emit updtScene();}


bool PolygonsList::exportCSV(QWidget* parent) {
	QString filename = QFileDialog::getSaveFileName(parent, tr("Save polygons"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/polygons.csv", "CSV files (.csv)");
	if(filename == "")
		return false;

	QFile data(filename);
	if(data.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream output(&data);

		for(int i=0; i<polygons.size(); i++) {
			output << polygons[i]->getName() << "_x;" << polygons[i]->getName() << "_y";
			if(i != polygons.size() - 1)
				output << ";";
		}
		output << endl;

		int maxIdx = 0;
		for(int i=0; i<polygons.count(); i++)
			maxIdx = qMax(maxIdx, polygons[i]->count());

		for(int i=0; i < maxIdx; i++) {
			for(int j=0; j<polygons.size(); j++) {
				if(i < polygons[j]->size()) {
					output << polygons[j]->at(i).x() << ";" << polygons[j]->at(i).y();
				} else {
					output << " ; ";
				}
				if(j != polygons.size() - 1)
					output << ";";
			}
			output << endl;
		}

	} else {
		QMessageBox::warning(parent, "Sukyan", tr("Cannot save the file due to a permission error"));
		return false;
	}

	pointsSaved = true;
	return true;
}

void PolygonsList::importCSV(QWidget* parent) {
	QString filename = QFileDialog::getOpenFileName(parent, "Sukyan", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Comma separated values (*.csv)"));
	if(filename == "")
		return;

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(parent, "Sukyan", tr("Cannot read the file : ") + file.errorString());
		return;
	}

	QStringList wordList;
	QStringList validPolygons;
	QList<int> validXIndex;

	QString line = file.readLine();
	wordList.append(line.split(';'));
	for(int i=0; i<wordList.size(); i++)
		wordList[i] = wordList[i].simplified();

	for(int i=0; i<wordList.size() - 1; i++) {
		if(wordList[i].right(2) != "_x" || wordList[i+1].right(2) != "_y")
			continue;

		if(wordList[i].left(wordList[i].size() - 2) != wordList[i+1].left(wordList[i].size() - 2))
			continue;

		if(wordList.contains(wordList[i].left(wordList[i].size() - 2))) {
			QMessageBox::warning(parent, "Sukyan", tr("Two polygons with the same name, only the first will be imported"));
		} else {
			validPolygons.append(wordList[i].left(wordList[i].size() - 2));
			validXIndex.append(i);
		}
	}

	if(validPolygons.size() == 0) {
		QMessageBox::warning(parent, "Sukyan", tr("No parsable polygons found"));
		return;
	}

	QList<class Polygon*> newPolygons;
	while(validPolygons.size()) {
		class Polygon* p = new Polygon("");
		QString name = validPolygons.takeAt(0);

		bool nameAlreadyExistsInNewPolygons = false;
		for(int i=0; i<newPolygons.size(); i++)
			if(newPolygons[i]->getName() == name)
				nameAlreadyExistsInNewPolygons = true;

		if(nameAlreadyExists(name) || nameAlreadyExistsInNewPolygons) {
			int idx = 1;
			bool ok = false;

			while(!ok) {
				idx++;
				if(!nameAlreadyExists(name + " (" + QString::number(idx) + ")"))
					ok = true;
			}

			name += " (" + QString::number(idx) + ")";
		}

		p->setName(name);
		newPolygons << p;
	}

	while(!file.atEnd()) {
		wordList.clear();

		QString line = file.readLine();
		wordList.append(line.split(';'));

		for(int i=0; i<validXIndex.size(); i++) {
			QPoint p;
			if(i+1 >= wordList.size())
				continue;

			if(wordList[validXIndex[i]].trimmed() == "")
				continue;

			p.setX(wordList[validXIndex[i]].toInt());
			p.setY(wordList[validXIndex[i] + 1].toInt());
			newPolygons[i]->append(p);
			newPolygons[i]->setCurrentPointIdx(0);
		}
	}

	polygons.append(newPolygons);
	setCurrentIndex(size() - 1);

	pointsSaved = false;
	updateDock();
	updateScene();
}
