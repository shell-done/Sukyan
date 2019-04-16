#include "image.h"

Image::Image(QString fp, PolygonsList* polyList, QWidget* parent) : QWidget(parent), filePath(fp) {
	scene = new Scene(fp, polyList);
	view = new View(scene);

    QGridLayout* layout = new QGridLayout(this);
    this->setLayout(layout);
    layout->addWidget(view, 0, 0);

    connect(view, SIGNAL(mousePositionChanged(QPoint)), this, SLOT(mousePositionChanged(QPoint)));
    connect(view, SIGNAL(wheelZoomValueChanged(int)), this, SLOT(wheelZoomChanged(int)));
	connect(scene, SIGNAL(contrastChanged(int, int)), this, SLOT(contrastSceneChanged(int, int)));
}


Image::~Image() {
	delete view;
	delete scene;
}

QPixmap* Image::pixmap() const {
	return scene->getBackgroundPixmap();
}

Sukyan::matrixInformation Image::getMatrix() {
    return view->getMatrix();
}

void Image::setMode(Sukyan::mode m) {
    if(m == Sukyan::CURSOR)
        view->setDragMode(QGraphicsView::ScrollHandDrag);
    else
        view->setDragMode(QGraphicsView::NoDrag);

    scene->setMode(m);
}
void Image::setTool(Sukyan::drawingTool t) {scene->setTool(t);}
void Image::setPen(QPen p) {scene->setPen(p);}
void Image::showDrawing(bool s) {scene->show(s);}
void Image::clearDrawing() {scene->clear();}
bool Image::save(bool keepFilePath) {
	QString path = filePath;
	QString fileName = QFileInfo(QFile(filePath).fileName()).baseName();

	if(!keepFilePath) {
		path = QFileDialog::getSaveFileName(this, tr("Save Image"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/" + fileName, tr("Portable Network Graphics (*.png);;Joint Photographic Experts Group (*.jpg);;Joint Photographic Experts Group (*.jpeg);;BitMap (*.bmp);;Portable PixMap (*.ppm);;XBitMap (*.xbm);;XPixMap (*.xpm)"));
		if(path == "")
			return false;
	}
	QPixmap toSavePxmp(scene->getBackgroundPixmap()->size());

	QPainter painter(&toSavePxmp);
	painter.drawPixmap(0, 0, *scene->getBackgroundPixmap());
	painter.drawPixmap(0, 0, *scene->getDrawingPixmap());
	painter.drawPixmap(0, 0, scene->getPolygonsPixmap());
	painter.end();

	if(toSavePxmp.save(path))
		return true;

	bool ok = false;

	while(!ok) {
		QMessageBox::warning(this, "Sukyan", "Error while saving the image, please choose another file path or file name");
		QString newPath = QFileDialog::getSaveFileName(this, tr("Save Image"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/" + fileName, tr("Portable Network Graphics (*.png);;Joint Photographic Experts Group (*.jpg);;Joint Photographic Experts Group (*.jpeg);;BitMap (*.bmp);;Portable PixMap (*.ppm);;XBitMap (*.xbm);;XPixMap (*.xpm)"));
		if(newPath == "")
			return false;

		ok = toSavePxmp.save(newPath);
	}
	return true;
}
bool Image::exportPoints() {
	return scene->savePolygons(this);
}
void Image::importPoints() {
	scene->importPolygons(this);
}

QString Image::getPath() {return filePath;}
QString Image::getFileName() {return QFileInfo(QFile(filePath).fileName()).fileName();}
bool Image::getImageSaved() {return scene->getImageSaved();}
bool Image::getPolygonsSaved() {return scene->getPolygonsSaved();}
PolygonsList* Image::getPolygonsList() {return scene->getPolygonsList();}


/*        SLOTS        */
void Image::mousePositionChanged(QPoint p) {
    Sukyan::cursorInformation cursorInfo;
	cursorInfo.position = p;

	if(p.x() >= 0 && p.x() < scene->getBackgroundPixmap()->width() && p.y() >= 0 && p.y() < scene->getBackgroundPixmap()->height())
		cursorInfo.pixelColor = QColor(scene->getBackgroundPixmap()->toImage().pixel(p));
    else
        cursorInfo.pixelColor = QColor(0, 0, 0, 0);

    emit cursorInformationChanged(cursorInfo);
}

void Image::rotationChanged(int r) {
    view->rotationChanged(r);
}

void Image::zoomChanged(int s) {
    view->zoomChanged(s);
}

void Image::wheelZoomChanged(int s) {
    emit wheelZoomValueChanged(s);
}

void Image::setContrast(int b, int c) {
	scene->setContrast(b, c);
}

void Image::contrastSceneChanged(int b, int c) {
	emit contrastChanged(b, c);
}
