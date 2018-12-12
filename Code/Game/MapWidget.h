#pragma once

#include "ui_MapWidget.h"

class DataSender;

class MapWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MapWidget(QWidget *parent = nullptr);

	static void LoadResource();

	void setDataSender(DataSender *sender) { dataSender = sender; }
	void updateImage();
	ScreenPoint getMousePos();
	void gameOver();
	void resetGame();

signals:
	void pauseGame();
	void quitGame();
	void actorDied();
	void hostQuit();
	void allDied();

protected:
	void paintEvent(QPaintEvent *evt) override;
	void keyPressEvent(QKeyEvent *evt) override;
	void keyReleaseEvent(QKeyEvent *evt) override;
	void mousePressEvent(QMouseEvent *evt) override;
	void mouseReleaseEvent(QMouseEvent *evt) override;
	void mouseMoveEvent(QMouseEvent *evt) override;
	void focusOutEvent(QFocusEvent *evt) override;

private:
	void decideBulletDirection();
	void setMousePos(const ScreenPoint &pos);
	void takeScreenShot();

private:
	static QImage gameOverImage;

	Ui::MapWidget ui;
	DataSender *dataSender;
	QPixmap imageToShow;
	ScreenPoint mousePos;
	QMutex mousePosMutex;
	qint8 keyJudge[4];
	bool clicked, over;
};
