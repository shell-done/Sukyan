#ifndef MARONG_H
#define MARONG_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QVector2D>
#include <QLayout>
#include <QTimer>
#include <QKeyEvent>
#include <QDebug>
#include <QIcon>
#include <cstdlib>
#include <ctime>

class Marong : public QWidget {
	Q_OBJECT

public:
	Marong();

protected:
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);
	void closeEvent(QCloseEvent*);

private:
	QGraphicsScene* scene;
	QGraphicsView* view;
	bool pause;
	QTimer* timer;

	const int sceneWidth = 1280;
	const int sceneHeight = 720;
	const int racketOffset = 25;
	const int racketHeight = 100;
	const int racketWidth = 15;

	QGraphicsRectItem* backScene;
	QGraphicsLineItem* topBorder;
	QGraphicsLineItem* bottomBorder;
	QGraphicsLineItem* rightBorder;
	QGraphicsLineItem* leftBorder;

	int racketSpeed;
	int leftRacketDir;
	QRect leftRacket;
	int rightRacketDir;
	QRect rightRacket;
	QGraphicsRectItem* rackets[2];

	int scores[2];
	QGraphicsTextItem* scoresText[2];

	int ballRadius;
	QGraphicsEllipseItem* ballEllipse;
	QGraphicsPixmapItem* marong;

	int ballSpeed;
	QVector2D ballDirection;

	void resetBall(int);
	void bump(int);

public slots:
	void play();

private slots:
	void updateScene();

signals:
	void hide();
};

#endif // MARONG_H
