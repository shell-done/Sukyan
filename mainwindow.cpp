#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    initDockWidget();
    initActions();
    initMenu();
    initToolbar();

    tempView = new QGraphicsView(this);
    setCentralWidget(tempView);

    images = new QStackedWidget();

    mode = Sukyan::CURSOR;
    tool = Sukyan::PEN;
    currentPen.setWidth(10);
    currentPen.setColor(Qt::black);

	qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch() / 1000));

    connect(dockExplorer, SIGNAL(currentRowChanged(int)), this, SLOT(changeImage(int)));
	connect(dockExplorer, SIGNAL(closeImage(int)), this, SLOT(closeImage(int)));
	connect(dockDrawing, SIGNAL(colorChanged(QColor)), this, SLOT(setPenColor(QColor)));
	connect(dockDrawing, SIGNAL(penWidthChanged(int)), this, SLOT(setPenWidth(int)));
	connect(dockDrawing, SIGNAL(displayChanged(bool)), this, SLOT(showDrawing(bool)));
	connect(dockDrawing, &DockDrawing::clear, [this](){
		for(int i=0; i<images->count(); i++)
			qobject_cast<Image*>(images->widget(i))->clearDrawing();
	});
	connect(dockImage, SIGNAL(ratioChanged(double)), dockSelection, SLOT(setSurfaceRatio(double)));

	contrastDialog = new ContrastDialog(this);
	contrastDialog->hide();

	copyPolygonsDialog = new CopyPolygonsDialog(this);
	copyPolygonsDialog->hide();
	connect(copyPolygonsDialog, SIGNAL(copy(QString, QStringList)), this, SLOT(copyPolygons(QString, QStringList)));

	splitDialog = new SplitDialog(this);

	setWindowIcon(QIcon(":/icons/ultrasound.png"));
}

/*        INITIALIZATION        */
void MainWindow::initDockWidget() {
    QDockWidget* dock = new QDockWidget(tr("Image"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockImage = new DockImage(dock);
    dock->setWidget(dockImage);
	dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
	action_dockImage = dock->toggleViewAction();
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	dock = new QDockWidget(tr("Drawing"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dockDrawing = new DockDrawing(dock);
	dock->setWidget(dockDrawing);
	dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
	dock->setVisible(false);
	action_dockDrawing = dock->toggleViewAction();
	addDockWidget(Qt::LeftDockWidgetArea, dock);

    dock = new QDockWidget(tr("Selection"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockSelection = new DockSelection(dock);
    dock->setWidget(dockSelection);
	dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
	action_dockSelection = dock->toggleViewAction();
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    dock = new QDockWidget(tr("Explorer"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dockExplorer = new DockExplorer(dock);
    dock->setWidget(dockExplorer);
	dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
	action_dockExplorer = dock->toggleViewAction();
    addDockWidget(Qt::RightDockWidgetArea, dock);

    dockImage->setDisabled(true);
	dockDrawing->setDisabled(true);
    dockSelection->setDisabled(true);
    dockExplorer->setDisabled(true);
}

void MainWindow::initActions() {
    action_openFile = new QAction(tr("&Open..."), this);
    action_openFile->setIcon(QIcon(":/icons/open_file.png"));
	action_openFile->setShortcut(QKeySequence("Ctrl+o"));
    connect(action_openFile, SIGNAL(triggered()), this, SLOT(openImage()));

	action_saveFile = new QAction(tr("&Save ..."), this);
	action_saveFile->setIcon(QIcon(":/icons/save_file.png"));
	action_saveFile->setShortcut(QKeySequence("Ctrl+s"));
	action_saveFile->setDisabled(true);
	connect(action_saveFile, &QAction::triggered, [this](){saveImage(images->currentIndex());});

	action_saveFileAs = new QAction(tr("&Save as..."), this);
	action_saveFileAs->setIcon(QIcon(":/icons/save_file_as.png"));
	action_saveFileAs->setShortcut(QKeySequence("Ctrl+shift+s"));
	action_saveFileAs->setDisabled(true);
	connect(action_saveFileAs, &QAction::triggered, [this](){saveImageAs(images->currentIndex());});

	action_quit = new QAction(tr("&Quit"), this);
	action_quit->setMenuRole(QAction::QuitRole);
	connect(action_quit, &QAction::triggered, qApp, &QApplication::quit);

	action_dockImage->setText(tr("Dock Image"));
	action_dockImage->setShortcut(QKeySequence("F5"));
	action_dockDrawing->setText(tr("Dock Drawing"));
	action_dockDrawing->setShortcut(QKeySequence("F6"));
	action_dockSelection->setText(tr("Dock Selection"));
	action_dockSelection->setShortcut(QKeySequence("F7"));
	action_dockExplorer->setText(tr("Dock Explorer"));
	action_dockExplorer->setShortcut(QKeySequence("F8"));

    action_modeCursor = new QAction(tr("&Cursor"), this);
    action_modeCursor->setIcon(QIcon(":/icons/mode_cursor.png"));
	action_modeCursor->setShortcut(QKeySequence("F1"));
    action_modeCursor->setCheckable(true);
    action_modeCursor->setChecked(true);
    connect(action_modeCursor, &QAction::triggered, [this](){setMode(Sukyan::CURSOR);});

    action_modeDrawing = new QAction(tr("&Drawing"), this);
    action_modeDrawing->setIcon(QIcon(":/icons/mode_drawing.png"));
	action_modeDrawing->setShortcut(QKeySequence("F2"));
    action_modeDrawing->setCheckable(true);
    connect(action_modeDrawing, &QAction::triggered, [this](){setMode(Sukyan::DRAWING);});

    action_modePolygon = new QAction(tr("&Polygon"), this);
    action_modePolygon->setIcon(QIcon(":/icons/mode_polygon.png"));
	action_modePolygon->setShortcut(QKeySequence("F3"));
    action_modePolygon->setCheckable(true);
    connect(action_modePolygon, &QAction::triggered, [this](){setMode(Sukyan::POLYGON);});

	action_modeContrast = new QAction(tr("&Contrast"), this);
	action_modeContrast->setIcon(QIcon(":/icons/mode_contrast.png"));
	action_modeContrast->setShortcut(QKeySequence("F4"));
	action_modeContrast->setCheckable(true);
	connect(action_modeContrast, &QAction::triggered, [this](){setMode(Sukyan::CONTRAST);});


    action_colorPicker = new QAction(tr("&Color picker"), this);
    QPixmap colorPickerPixmap(100, 100);
    colorPickerPixmap.fill(Qt::transparent);

    QPainter painter(&colorPickerPixmap);
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect(10, 10, 80, 80);
    painter.end();

    action_colorPicker->setIcon(colorPickerPixmap);

	connect(action_colorPicker, &QAction::triggered, [this](){QColor c = QColorDialog::getColor(currentPen.color(), this); setPenColor(c);});

    action_toolPen = new QAction(tr("&Pen"), this);
    action_toolPen->setIcon(QIcon(":/icons/tool_pen.png"));
	action_toolPen->setShortcut(QKeySequence("P"));
    action_toolPen->setCheckable(true);
    connect(action_toolPen, &QAction::triggered, [this](){setTool(Sukyan::PEN);});

    action_toolLine = new QAction(tr("&Line"), this);
    action_toolLine->setIcon(QIcon(":/icons/tool_line.png"));
	action_toolLine->setShortcut(QKeySequence("L"));
    action_toolLine->setCheckable(true);
    connect(action_toolLine, &QAction::triggered, [this](){setTool(Sukyan::LINE);});

    action_toolRect = new QAction(tr("&Rectangle"), this);
    action_toolRect->setIcon(QIcon(":/icons/tool_rectangle.png"));
	action_toolRect->setShortcut(QKeySequence("R"));
    action_toolRect->setCheckable(true);
    connect(action_toolRect, &QAction::triggered, [this](){setTool(Sukyan::RECTANGLE);});

    action_toolEllipse = new QAction(tr("&Ellipse"), this);
    action_toolEllipse->setIcon(QIcon(":/icons/tool_ellipse.png"));
	action_toolEllipse->setShortcut(QKeySequence("E"));
    action_toolEllipse->setCheckable(true);
    connect(action_toolEllipse, &QAction::triggered, [this](){setTool(Sukyan::ELLIPSE);});

    action_toolRubber = new QAction(tr("&Rubber"), this);
    action_toolRubber->setIcon(QIcon(":/icons/tool_rubber.png"));
	action_toolRubber->setShortcut(QKeySequence("D"));
    action_toolRubber->setCheckable(true);
    connect(action_toolRubber, &QAction::triggered, [this](){setTool(Sukyan::RUBBER);});

    action_toolBucket = new QAction(tr("&Bucket"), this);
    action_toolBucket->setIcon(QIcon(":/icons/tool_bucket.png"));
	action_toolBucket->setShortcut(QKeySequence("B"));
    action_toolBucket->setCheckable(true);
    connect(action_toolBucket, &QAction::triggered, [this](){setTool(Sukyan::BUCKET);});

    modeGroup = new QActionGroup(this);
    modeGroup->addAction(action_modeCursor);
    modeGroup->addAction(action_modeDrawing);
    modeGroup->addAction(action_modePolygon);
	modeGroup->addAction(action_modeContrast);

    toolGroup = new QActionGroup(this);
    toolGroup->addAction(action_colorPicker);
    toolGroup->addAction(action_toolPen);
    toolGroup->addAction(action_toolLine);
    toolGroup->addAction(action_toolRect);
    toolGroup->addAction(action_toolEllipse);
    toolGroup->addAction(action_toolRubber);
    toolGroup->addAction(action_toolBucket);

	action_contrast = new QAction(tr("Brightness / Contrast"));
	action_contrast->setDisabled(true);
	action_importPoints = new QAction(tr("Import points"));
	action_importPoints->setDisabled(true);
	action_exportPoints = new QAction(tr("Export points"));
	action_exportPoints->setDisabled(true);
	action_copyPoints = new QAction(tr("Copy polygons to..."));
	action_copyPoints->setDisabled(true);
	action_splitPolygon = new QAction(tr("Sample polygon..."));
	action_splitPolygon->setDisabled(true);

	connect(action_contrast, &QAction::triggered, [this]() {contrastDialog->show();});
	connect(action_importPoints, &QAction::triggered, [this]() {qobject_cast<Image*>(images->currentWidget())->importPoints();});
	connect(action_exportPoints, &QAction::triggered, [this]() {qobject_cast<Image*>(images->currentWidget())->exportPoints();});
	connect(action_copyPoints, &QAction::triggered, [this]() {
		Image* currentImage = qobject_cast<Image*>(images->currentWidget());
		PolygonsList* pls = currentImage->getPolygonsList();

		QStringList ils;
		for(int i=0; i<images->count(); i++) {
			if(qobject_cast<Image*>(images->widget(i)) == currentImage)
				continue;

			ils << qobject_cast<Image*>(images->widget(i))->getFileName();
		}

		if(ils.isEmpty()) {
			QMessageBox::information(this, tr("Sukyan"), tr("At least 2 images must be opened to copy points"));
			return;
		}
		if(pls->size() == 0) {
			QMessageBox::information(this, tr("Sukyan"), tr("No polygons to copy"));
			return;
		}
		copyPolygonsDialog->openDialog(pls, ils);
	});
	connect(action_splitPolygon, &QAction::triggered, [this]() {
		splitDialog->openDialog(qobject_cast<Image*>(images->currentWidget())->getPolygonsList());
	});

	action_about = new QAction(tr("&About ..."), this);
	connect(action_about, &QAction::triggered, [this](){QMessageBox::about(this, tr("About Sukyan"), tr(
																			   "<b>About Sukyan</b><br/> \
																			   Software made by Alexandre THOMAS in CIR2 at ISEN Yncréa Ouest Rennes <br/> \
																			   <a href='alexandre-thomas.fr'>alexandre-thomas.fr</a> <br/> <br/>\
																			   Icons by icons8.com"
																			   ));});

    action_aboutQt = new QAction(tr("&About Qt..."), this);
    connect(action_aboutQt, &QAction::triggered, [this](){QMessageBox::aboutQt(this, tr("About Qt"));});

	marong = new Marong();
	action_marong = new QAction(tr("&Marong"), this);
	action_marong->setIcon(QPixmap(":/icons/marong.png"));
	action_marong->setShortcut(QKeySequence("Ctrl+Shift+M"));
	addAction(action_marong);
	connect(action_marong, SIGNAL(triggered()), marong, SLOT(play()));
}

void MainWindow::initMenu() {
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(action_openFile);
    fileMenu->addAction(action_saveFile);
	fileMenu->addAction(action_saveFileAs);
	fileMenu->addSeparator();
	fileMenu->addAction(action_quit);

    QMenu* displayMenu = menuBar()->addMenu(tr("&Display"));
	displayMenu->addAction(action_dockImage);
	displayMenu->addAction(action_dockDrawing);
	displayMenu->addAction(action_dockSelection);
	displayMenu->addAction(action_dockExplorer);

    QMenu* toolMenu = menuBar()->addMenu(tr("&Tool"));
    toolMenu->addAction(action_modeCursor);
    toolMenu->addAction(action_modeDrawing);
    toolMenu->addAction(action_modePolygon);
	toolMenu->addAction(action_modeContrast);
    toolMenu->addSeparator();
    toolMenu->addAction(action_toolPen);
    toolMenu->addAction(action_toolLine);
    toolMenu->addAction(action_toolRect);
    toolMenu->addAction(action_toolEllipse);
    toolMenu->addAction(action_toolRubber);
    toolMenu->addAction(action_toolBucket);

	QMenu* imageMenu = menuBar()->addMenu(tr("&Image"));
	imageMenu->addAction(action_contrast);
	imageMenu->addSeparator();
	imageMenu->addAction(action_importPoints);
	imageMenu->addAction(action_exportPoints);
	imageMenu->addAction(action_copyPoints);
	imageMenu->addAction(action_splitPolygon);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(action_about);
    helpMenu->addAction(action_aboutQt);
}

void MainWindow::initToolbar() {
    QToolBar* mainToolbar = addToolBar("");
    mainToolbar->setMovable(false);
    mainToolbar->setFloatable(false);

    mainToolbar->addAction(action_openFile);
    mainToolbar->addAction(action_saveFile);
    mainToolbar->addSeparator();
    mainToolbar->addAction(action_modeCursor);
    mainToolbar->addAction(action_modeDrawing);
    mainToolbar->addAction(action_modePolygon);
	mainToolbar->addAction(action_modeContrast);

    QToolBar* toolToolbar = addToolBar("");
    toolToolbar->setMovable(false);
    toolToolbar->setFloatable(false);
    toolToolbar->addAction(action_colorPicker);
    toolToolbar->addAction(action_toolPen);
    toolToolbar->addAction(action_toolLine);
    toolToolbar->addAction(action_toolRect);
    toolToolbar->addAction(action_toolEllipse);
    toolToolbar->addAction(action_toolRubber);
    toolToolbar->addAction(action_toolBucket);
}



void MainWindow::connectImage() {
    Image* currentImage = qobject_cast<Image*>(images->currentWidget());
    connect(currentImage, SIGNAL(cursorInformationChanged(Sukyan::cursorInformation)), dockImage, SLOT(cursorInformationChanged(Sukyan::cursorInformation)));
    connect(dockImage, SIGNAL(rotationChanged(int)), currentImage, SLOT(rotationChanged(int)));
    connect(dockImage, SIGNAL(zoomChanged(int)), currentImage, SLOT(zoomChanged(int)));
    connect(currentImage, SIGNAL(wheelZoomValueChanged(int)), dockImage, SLOT(zoomValueChanged(int)));
	connect(contrastDialog, SIGNAL(contrastChanged(int, int)), currentImage, SLOT(setContrast(int, int)));
	connect(currentImage, SIGNAL(contrastChanged(int, int)), contrastDialog, SLOT(setValues(int, int)));
}

void MainWindow::disconnectImage() {
    Image* currentImage = qobject_cast<Image*>(images->currentWidget());
    disconnect(currentImage, SIGNAL(cursorInformationChanged(Sukyan::cursorInformation)), dockImage, SLOT(cursorInformationChanged(Sukyan::cursorInformation)));
    disconnect(dockImage, SIGNAL(rotationChanged(int)), currentImage, SLOT(rotationChanged(int)));
    disconnect(dockImage, SIGNAL(zoomChanged(int)), currentImage, SLOT(zoomChanged(int)));
    disconnect(currentImage, SIGNAL(wheelZoomValueChanged(int)), dockImage, SLOT(zoomValueChanged(int)));
	disconnect(contrastDialog, SIGNAL(contrastChanged(int, int)), currentImage, SLOT(setContrast(int, int)));
	disconnect(currentImage, SIGNAL(contrastChanged(int, int)), contrastDialog, SLOT(setValues(int, int)));
}

void MainWindow::setMode(Sukyan::mode m) {
    mode = m;
    for(int i=0; i<images->count(); i++)
        qobject_cast<Image*>(images->widget(i))->setMode(mode);
}

void MainWindow::setTool(Sukyan::drawingTool t) {
    tool = t;
    for(int i=0; i<images->count(); i++)
        qobject_cast<Image*>(images->widget(i))->setTool(tool);

	action_modeDrawing->trigger();
}



/*        SLOTS        */
void MainWindow::openImage() {
	QStringList files = QFileDialog::getOpenFileNames(this, tr("Open Images"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), tr("Image Files (*.png *.jpg *.jpeg *.bmp *.ppm *.xbm *.xpm)"));

	for(int i=0; i<files.length(); i++) {
		QString filePath = files[i];

		if(filePath.trimmed() == "")
			continue;

		if(QPixmap(filePath).isNull()) {
			QMessageBox::warning(this, "Sukyan", tr("Error while loading image. Check if the path is correct and if the image is not corrupted"));
			continue;
		}

		for(int i=0; i<images->count(); i++) {
			Image* img = qobject_cast<Image*>(images->widget(i));
			if(filePath == img->getPath()) {
				QMessageBox::warning(this, "Sukyan", tr("Error while loading image : file already open"));
				continue;
			}
		}

		if(!tempView)
			disconnectImage();

		PolygonsList* polyList = new PolygonsList();
		dockSelection->setCurrentPolygonList(polyList);


		Image* newImg = new Image(filePath, polyList, this);
		newImg->setPen(currentPen);
		newImg->setMode(mode);
		images->addWidget(newImg);
		images->setCurrentIndex(images->count() - 1);
		connectImage();

		setCentralWidget(images);
		dockExplorer->addThumbnail(newImg->pixmap(), QFileInfo(QFile(filePath).fileName()).fileName());

		dockImage->rotationValueChanged(0);
		dockImage->zoomValueChanged(100);

		//When the first image is loaded, we delete the tempView and enable dock widgets
		if(tempView) {
			delete tempView;
			tempView = nullptr;

			action_saveFile->setEnabled(true);
			action_saveFileAs->setEnabled(true);
			dockImage->setEnabled(true);
			dockDrawing->setEnabled(true);
			dockSelection->setEnabled(true);
			dockExplorer->setEnabled(true);
			action_contrast->setEnabled(true);
			action_importPoints->setEnabled(true);
			action_exportPoints->setEnabled(true);
			action_copyPoints->setEnabled(true);
			action_splitPolygon->setEnabled(true);
		}
	}
}

void MainWindow::saveImage(int idx) {
	qobject_cast<Image*>(images->widget(idx))->save(true);
}

void MainWindow::saveImageAs(int idx) {
	qobject_cast<Image*>(images->widget(idx))->save(false);
}

void MainWindow::closeImage(int idx) {
	if(images->count() == 1)
		return;

	if(!qobject_cast<Image*>(images->widget(idx))->getImageSaved()) {
		QMessageBox msgBox(this);
		msgBox.setText("Sukyan");
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setInformativeText(tr("The image has been modified. Do you want to save your changes?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.setMinimumSize(325, 125);

		int ret = msgBox.exec();
		if(ret == QMessageBox::Save)
			if(!qobject_cast<Image*>(images->widget(idx))->save(true))
				return;

		if(ret == QMessageBox::Cancel)
			return;
	}
	if(!qobject_cast<Image*>(images->widget(idx))->getPolygonsSaved()) {
		QMessageBox msgBox(this);
		msgBox.setText("Sukyan");
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setInformativeText(tr("Polygons have been modified. Do you want to export your changes?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.setMinimumSize(325, 125);

		int ret = msgBox.exec();
		if(ret == QMessageBox::Save)
			if(!qobject_cast<Image*>(images->widget(idx))->exportPoints())
				return;

		if(ret == QMessageBox::Cancel)
			return;
	}


	int newIdx = idx;
	if(newIdx > 0)
		newIdx--;

	dockSelection->disconnectCurrentPolygonList();

	Image* img = qobject_cast<Image*>(images->widget(idx));
	images->removeWidget(img);
	delete img;

	dockExplorer->removeThumbnail(idx);
	dockExplorer->setCurrentThumbnail(newIdx);

	changeImage(newIdx);
}

void MainWindow::changeImage(int idx) {
    disconnectImage();

    images->setCurrentIndex(idx);

    connectImage();

    Sukyan::matrixInformation mtrxInfo = qobject_cast<Image*>(images->currentWidget())->getMatrix();
    dockImage->rotationValueChanged(mtrxInfo.rotation);
    dockImage->zoomValueChanged(mtrxInfo.scale);

	dockSelection->setCurrentPolygonList(qobject_cast<Image*>(images->currentWidget())->getPolygonsList());
}

void MainWindow::copyPolygons(QString dest, QStringList polygonsToCopy) {
	for(int i=0; i<images->count(); i++) {
		if(qobject_cast<Image*>(images->widget(i))->getFileName() == dest) {
			PolygonsList* srcPl = qobject_cast<Image*>(images->currentWidget())->getPolygonsList();
			PolygonsList* destPl = qobject_cast<Image*>(images->widget(i))->getPolygonsList();

			for(int i=0; i<srcPl->size(); i++) {
				if(polygonsToCopy.contains(srcPl->at(i).getName())) {
					destPl->newPolygon(srcPl->at(i));
				}
			}

			destPl->updateScene();
			destPl->setCurrentIndex(destPl->size() - 1);
		}
	}
}

void MainWindow::setPenColor(QColor c) {
	currentPen.setColor(c);

	QPixmap colorPickerPixmap(100, 100);
	colorPickerPixmap.fill(Qt::transparent);

	QPainter painter(&colorPickerPixmap);
	painter.setBrush(QBrush(c));
	painter.drawRect(10, 10, 80, 80);
	painter.end();

	action_colorPicker->setIcon(colorPickerPixmap);
	dockDrawing->setColor(currentPen.color());

	for(int i=0; i<images->count(); i++)
		qobject_cast<Image*>(images->widget(i))->setPen(currentPen);
}

void MainWindow::setPenWidth(int s) {
	currentPen.setWidth(s);
	for(int i=0; i<images->count(); i++)
		qobject_cast<Image*>(images->widget(i))->setPen(currentPen);
}

void MainWindow::showDrawing(bool b) {
	for(int i=0; i<images->count(); i++)
		qobject_cast<Image*>(images->widget(i))->showDrawing(b);
}
