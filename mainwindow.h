#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "core.h"

#include "dockimage.h"
#include "dockdrawing.h"
#include "dockselection.h"
#include "dockexplorer.h"

#include "contrast.h"
#include "image.h"
#include "marong.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);


private:
    /*        UI Initialization        */
    //Docks initialization
    DockImage* dockImage;
	DockDrawing* dockDrawing;
    DockSelection* dockSelection;
    DockExplorer* dockExplorer;
    void initDockWidget();

    //Actions, menu and toolbar initialization
    //File
    QAction* action_openFile;
    QAction* action_saveFile;
	QAction* action_saveFileAs;
	QAction* action_quit;

    //Display
	QAction* action_dockImage;
	QAction* action_dockDrawing;
	QAction* action_dockSelection;
	QAction* action_dockExplorer;

    //Tool
    QActionGroup* modeGroup;
    QAction* action_modeCursor;
    QAction* action_modeDrawing;
    QAction* action_modePolygon;
	QAction* action_modeContrast;
    QActionGroup* toolGroup;
    QAction* action_colorPicker;
    QAction* action_toolPen;
    QAction* action_toolLine;
    QAction* action_toolRect;
    QAction* action_toolEllipse;
    QAction* action_toolRubber;
    QAction* action_toolBucket;

	//Image
	QAction* action_contrast;
	QAction* action_importPoints;
	QAction* action_exportPoints;

    //About
	QAction* action_about;
    QAction* action_aboutQt;
    void initActions();
    void initMenu();
    void initToolbar();

	QAction* action_marong;

    //End of initialization
    QGraphicsView* tempView; //Used to have a view until the user load an image



    /*        Loading images        */
    //Loaded images
    QStackedWidget* images;
    void connectImage();
    void disconnectImage();


    /*        Use        */
    //User tools
    Sukyan::mode mode;
    Sukyan::drawingTool tool;
    QPen currentPen;
    void setMode(Sukyan::mode mode);
    void setTool(Sukyan::drawingTool tool);

	Marong* marong;
	ContrastDialog* contrastDialog;

private slots:
    void openImage();
	void saveImage(int);
	void saveImageAs(int);
	void closeImage(int);
    void changeImage(int idx);

	void setPenColor(QColor c);
	void setPenWidth(int);
	void showDrawing(bool);
};

#endif // MAINWINDOW_H
