#ifndef IMAGE_H
#define IMAGE_H

#include <QObject>
#include <QtWidgets>
#include "core.h"
#include "scene.h"
#include "view.h"

class Image : public QWidget {
    Q_OBJECT

public:
	explicit Image(QString filePath, PolygonsList* polygons, QWidget *parent = nullptr);
    ~Image();

	QPixmap* pixmap() const;
    Sukyan::matrixInformation getMatrix();

    void setMode(Sukyan::mode mode);
    void setTool(Sukyan::drawingTool tool);
    void setPen(QPen pen);
	void showDrawing(bool s);
	void clearDrawing();

	bool save(bool keepFilePath = true);
	bool exportPoints();
	void importPoints();

	QString getPath();
	QString getFileName();
	bool getImageSaved();
	bool getPolygonsSaved();
	PolygonsList* getPolygonsList();

private:
    QString filePath;

    Scene* scene;
    View* view;


private slots:
    void mousePositionChanged(QPoint);
    void rotationChanged(int);
    void zoomChanged(int);
	void wheelZoomChanged(int);
	void setContrast(int, int);
	void contrastSceneChanged(int, int);

signals:
    void cursorInformationChanged(Sukyan::cursorInformation);
    void wheelZoomValueChanged(int);
	void contrastChanged(int, int);
};

#endif // IMAGE_H
