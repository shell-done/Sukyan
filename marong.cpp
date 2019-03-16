#include "marong.h"


Marong::Marong() : QWidget() {
	QHBoxLayout* layout = new QHBoxLayout(this);
	scene = new QGraphicsScene(QRect(0, 0, sceneWidth, sceneHeight));
	view = new QGraphicsView(scene);
	scene->setBackgroundBrush(QBrush(Qt::white));

	setFixedSize(1300, 740);
	layout->addWidget(view);
	layout->setMargin(0);
	setLayout(layout);

	setFocusPolicy(Qt::StrongFocus);

	pause = true;
	ballSpeed = 10;
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateScene()));
	timer->start(16);

	backScene = scene->addRect(0, 0, sceneWidth, sceneHeight);
	backScene->setBrush(Qt::black);

	topBorder = scene->addLine(0, 0, sceneWidth, 0);
	bottomBorder = scene->addLine(0, sceneHeight, sceneWidth, sceneHeight);
	leftBorder = scene->addLine(0, 0, 0, sceneHeight);
	rightBorder = scene->addLine(sceneWidth, 0, sceneWidth, sceneHeight);

	QGraphicsRectItem* l = scene->addRect(sceneWidth/2 - 3, 0, 6, sceneHeight);
	l->setBrush(QBrush(Qt::white));

	QFont f;
	f.setPointSize(30);
	scoresText[0] = scene->addText("0");
	scoresText[0]->setDefaultTextColor(Qt::white);
	scoresText[0]->setPos(sceneWidth/2 - 45 - scoresText[0]->boundingRect().width(), 50);
	scoresText[0]->setFont(f);
	scoresText[1] = scene->addText("0");
	scoresText[1]->setDefaultTextColor(Qt::white);
	scoresText[1]->setPos(sceneWidth/2 + 30, 50);
	scoresText[1]->setFont(f);

	racketSpeed = 4;
	leftRacket = QRect(racketOffset, sceneHeight/2 - racketHeight/2, racketWidth, racketHeight);
	rightRacket = QRect(sceneWidth - racketWidth - racketOffset, sceneHeight/2 - racketHeight/2, racketWidth, racketHeight);

	leftRacketDir = 0;
	rightRacketDir = 0;
	scores[0] = 0;
	scores[1] = 0;

	rackets[0] = scene->addRect(leftRacket);
	rackets[0]->setBrush(QBrush(Qt::white));
	rackets[1] = scene->addRect(rightRacket);
	rackets[1]->setBrush(QBrush(Qt::white));

	ballRadius = 50;
	ballEllipse = scene->addEllipse(sceneWidth/2 - ballRadius/2, sceneHeight/2 - ballRadius/2, ballRadius, ballRadius);
	ballEllipse->setRect(sceneWidth/2 - ballRadius, sceneHeight/2 - ballRadius, ballRadius, ballRadius);
	ballEllipse->setBrush(QBrush(Qt::white));

	QPixmap pxm(":icons/marong.png");
	pxm = pxm.scaled(static_cast<int>(1.5*ballRadius), static_cast<int>(1.5*ballRadius));
	marong = scene->addPixmap(pxm);
	marong->setPos(ballEllipse->scenePos());
	marong->setTransformOriginPoint(QPointF(marong->boundingRect().width()/2, marong->boundingRect().height()/2));

	srand(static_cast<unsigned int>(time(nullptr)));


	float dirX = static_cast<float>(rand() * 1. / (2*RAND_MAX) + 0.5);
	float dirY = static_cast<float>(sqrt(static_cast<double>(1 - dirX*dirX)));
	if(rand() * 1. / RAND_MAX > 0.5)
		dirX *= (-1);
	if(rand() * 1. / RAND_MAX > 0.5)
		dirY *= (-1);

	ballDirection.setX(dirX);
	ballDirection.setY(dirY);

	setWindowTitle("Marong");
	setWindowIcon(QIcon(":/icons/marong.png"));
}

void Marong::keyPressEvent(QKeyEvent * event) {
	if(event->key() == Qt::Key_Z)
		leftRacketDir = - 1;
	else if(event->key() == Qt::Key_S)
		leftRacketDir = + 1;

	if(event->key() == Qt::Key_P)
		rightRacketDir = - 1;
	else if(event->key() == Qt::Key_M)
		rightRacketDir = + 1;

	QWidget::keyPressEvent(event);
}

void Marong::keyReleaseEvent(QKeyEvent * event) {
	if((event->key() == Qt::Key_Z && leftRacketDir == -1) || (event->key() == Qt::Key_S && leftRacketDir == 1))
		leftRacketDir = 0;

	if((event->key() == Qt::Key_P && rightRacketDir == -1) || (event->key() == Qt::Key_M && rightRacketDir == 1))
		rightRacketDir = 0;

	QWidget::keyReleaseEvent(event);
}

void Marong::updateScene() {
	if(pause)
		return;

	if(leftRacket.top() > 0 && leftRacketDir == -1)
		leftRacket.moveTop(leftRacket.top() - racketSpeed);
	else if(leftRacket.top() < sceneHeight - racketHeight && leftRacketDir == 1)
		leftRacket.moveTop(leftRacket.top() + racketSpeed);

	rackets[0]->setRect(leftRacket);

	if(rightRacket.y() > 0 && rightRacketDir == -1)
		rightRacket.moveTop(rightRacket.y() - racketSpeed);
	else if(rightRacket.y() < sceneHeight - racketHeight && rightRacketDir == 1)
		rightRacket.moveTop(rightRacket.y() + racketSpeed);

	rackets[1]->setRect(rightRacket);

	if((topBorder->collidesWithItem(ballEllipse) && ballDirection.y() < 0)|| (bottomBorder->collidesWithItem(ballEllipse) && ballDirection.y() > 0))
		ballDirection.setY(ballDirection.y()*(-1));

	if(leftBorder->collidesWithItem(ballEllipse))
		resetBall(0);
	else if(rightBorder->collidesWithItem(ballEllipse))
		resetBall(1);

	if(rackets[0]->collidesWithItem(ballEllipse) && ballDirection.x() < 0)
		bump(0);
	else if(rackets[1]->collidesWithItem(ballEllipse) && ballDirection.x() > 0)
		bump(1);

	ballEllipse->setPos(ballEllipse->pos() + ballDirection.toPointF()*ballSpeed);
	marong->setPos(ballEllipse->scenePos() + QPoint(sceneWidth/2 - ballRadius/2, sceneHeight/2 - ballRadius/2) - QPoint(ballRadius - 10, ballRadius - 10));
	marong->setRotation(1+marong->rotation());
}

void Marong::resetBall(int p) {
	ballEllipse->setPos(0, 0);

	float dirX = static_cast<float>(rand() * 1. / (2*RAND_MAX) + 0.5);
	float dirY = static_cast<float>(sqrt(static_cast<double>(1 - dirX*dirX)));
	if(rand() * 1. / RAND_MAX > 0.5)
		dirX *= (-1);
	if(rand() * 1. / RAND_MAX > 0.5)
		dirY *= (-1);

	ballDirection.setX(dirX);
	ballDirection.setY(dirY);

	scores[p]++;
	scoresText[1]->setPlainText(QString::number(scores[0]));
	scoresText[0]->setPlainText(QString::number(scores[1]));
}

void Marong::bump(int p) {
	float dirX = static_cast<float>(rand() * 1. / (2*RAND_MAX) + 0.5);
	float dirY = static_cast<float>(sqrt(static_cast<double>(1 - dirX*dirX)));
	if(rand() * 1. / RAND_MAX > 0.5)
		dirY *= (-1);

	if(p == 1)
		dirX *= (-1);
	if(ballDirection.y()/qAbs(ballDirection.y()) > 0)
		dirY *= (-1);

	ballDirection.setX(dirX);
	ballDirection.setY(dirY);

	int r = static_cast<int>(rand() * 255. / (2*RAND_MAX));
	int g = static_cast<int>(rand() * 255. / (2*RAND_MAX));
	int b = static_cast<int>(rand() * 255. / (2*RAND_MAX));
	backScene->setBrush(QBrush(QColor(r, g, b)));
}

void Marong::closeEvent(QCloseEvent* event) {
	Q_UNUSED(event);
	pause = true;
	hide();
}

void Marong::play() {
	pause = false;
	show();
}
