#include "stdafx.h"
#include "global_vars.h"
#include "MapWidget.h"
#include "DataManager.h"
#include "GameDisplayer.h"

QImage MapWidget::gameOverImage;

MapWidget::MapWidget(QWidget *parent)
	: QWidget(parent), dataSender(nullptr), keyJudge{ 0 }, clicked(false), over(false)
{
	ui.setupUi(this);
	mapWidget = this;
	setMouseTracking(true);
}

void MapWidget::LoadResource()
{
	QString source_file_name = "../images/GameOver.png";
	gameOverImage.load(source_file_name);
	if (gameOverImage.isNull())
		throw QString(u8"无法打开文件%1!").arg(source_file_name);
}

void MapWidget::updateImage()
{
	if (gameDisplayer == nullptr)
		return;
	gameDisplayer->imageMutex.lock();
	imageToShow.swap(gameDisplayer->imageToShow);
	gameDisplayer->imageMutex.unlock();
	if (clicked)
		decideBulletDirection();
	update();
}

ScreenPoint MapWidget::getMousePos()
{
	mousePosMutex.lock();
	ScreenPoint tmp = mousePos;
	mousePosMutex.unlock();
	return tmp;
}

void MapWidget::gameOver()
{
	over = true;
	update();
}

void MapWidget::resetGame()
{
	over = false;
}

void MapWidget::paintEvent(QPaintEvent *evt)
{
	if (imageToShow.isNull())
		return;

	QPainter painter(this);
	int w = width(), h = height();

	// 画Game Over
	if (over)
	{
		QImage tmp = gameOverImage.scaledToWidth(imageToShow.width() * 4 / 5);
		QPainter painter2(&imageToShow);
		painter2.drawImage(imageToShow.width() * 3 / 10, (imageToShow.height() - tmp.height()) / 2, tmp);
	}

	// 根据16:9的比例调整图片大小，并居中显示画好的图片
	if (w == imageToShow.width() && h == imageToShow.height())
	{
		painter.drawPixmap(0, 0, imageToShow);
	}
	else if (w * 9 > h * 16)
	{
		QPixmap tmp = imageToShow.scaledToHeight(h);
		painter.drawPixmap((w - tmp.width()) / 2, 0, tmp);
	}
	else
	{
		QPixmap tmp = imageToShow.scaledToWidth(w);
		painter.drawPixmap(0, (h - tmp.height()) / 2, tmp);
	}
}

void MapWidget::keyPressEvent(QKeyEvent *evt)
{
	KeyState test_key_press_signal_bundle(keyJudge[2] + keyJudge[3], keyJudge[0] + keyJudge[1]);

	switch (evt->key())
	{
	case Qt::Key_W:
		if (keyJudge[0] == 0)
		{
			test_key_press_signal_bundle.y = 1;
			keyJudge[0] = 1;
			keyJudge[1] = 0;
		}
		dataSender->sendKeyState(test_key_press_signal_bundle);
		return evt->accept();
	case Qt::Key_S:
		if (keyJudge[1] == 0)
		{
			test_key_press_signal_bundle.y = -1;
			keyJudge[1] = -1;
			keyJudge[0] = 0;
		}
		dataSender->sendKeyState(test_key_press_signal_bundle);
		return evt->accept();
	case Qt::Key_A:
		if (keyJudge[2] == 0)
		{
			test_key_press_signal_bundle.x = -1;
			keyJudge[2] = -1;
			keyJudge[3] = 0;
		}
		dataSender->sendKeyState(test_key_press_signal_bundle);
		return evt->accept();
	case Qt::Key_D:
		if (keyJudge[3] == 0)
		{
			test_key_press_signal_bundle.x = 1;
			keyJudge[3] = 1;
			keyJudge[2] = 0;
		}
		dataSender->sendKeyState(test_key_press_signal_bundle);
		return evt->accept();
	case Qt::Key_P:
		emit pauseGame();
		return evt->accept();
	case Qt::Key_Escape:
		if (QMessageBox::information(this, u8"提醒", u8"确定要回到主菜单吗？", u8"确定", u8"取消") == 0)
			emit quitGame();
		return evt->accept();
	case Qt::Key_F10:
		takeScreenShot();
		return evt->accept();
	default:
		return evt->ignore();
	}
}

void MapWidget::keyReleaseEvent(QKeyEvent *evt)
{
	KeyState test_key_release_signal_bundle(keyJudge[2] + keyJudge[3], keyJudge[0] + keyJudge[1]);

	switch (evt->key())
	{
	case Qt::Key_W:
		if (keyJudge[0] == 1)
		{
			test_key_release_signal_bundle.y = 0;
			keyJudge[0] = 0;
		}
		dataSender->sendKeyState(test_key_release_signal_bundle);
		return evt->accept();
	case Qt::Key_S:
		if (keyJudge[1] == -1)
		{
			test_key_release_signal_bundle.y = 0;
			keyJudge[1] = 0;
		}
		dataSender->sendKeyState(test_key_release_signal_bundle);
		return evt->accept();
	case Qt::Key_A:
		if (keyJudge[2] == -1)
		{
			test_key_release_signal_bundle.x = 0;
			keyJudge[2] = 0;
		}
		dataSender->sendKeyState(test_key_release_signal_bundle);
		return evt->accept();
	case Qt::Key_D:
		if (keyJudge[3] == 1)
		{
			test_key_release_signal_bundle.x = 0;
			keyJudge[3] = 0;
		}
		dataSender->sendKeyState(test_key_release_signal_bundle);
		return evt->accept();
	default:
		return evt->ignore();
	}
}

void MapWidget::mousePressEvent(QMouseEvent *evt)
{
	setMousePos(ScreenPoint(evt->pos().x(), evt->pos().y()));
	if (evt->button() == Qt::LeftButton)
	{
		clicked = true;
		decideBulletDirection();
	}
}

void MapWidget::mouseReleaseEvent(QMouseEvent *evt)
{
	setMousePos(ScreenPoint(evt->pos().x(), evt->pos().y()));
	if (evt->button() == Qt::LeftButton)
	{
		clicked = false;
		dataSender->sendMouseReleaseInfo();		
	}
}

void MapWidget::mouseMoveEvent(QMouseEvent *evt)
{
	setMousePos(ScreenPoint(evt->pos().x(), evt->pos().y()));
}

void MapWidget::focusOutEvent(QFocusEvent *evt)
{
	memset(keyJudge, 0, sizeof keyJudge);
	clicked = false;
	dataSender->sendKeyState(KeyState(0, 0));
	dataSender->sendMouseReleaseInfo();
	update();
}

void MapWidget::decideBulletDirection()
{
	if (mousePos.x > imageToShow.width() || mousePos.x < 0)
		return dataSender->sendMouseReleaseInfo();
	if (mousePos.y > imageToShow.height() || mousePos.y < 0)
		return dataSender->sendMouseReleaseInfo();
	S2M_Transformation s2mt = gameDisplayer->getScreenToMapTrans();
	dataSender->sendMousePressInfo(s2mt(mousePos));
}

void MapWidget::setMousePos(const ScreenPoint &click_pos)
{
	int x0 = width() - imageToShow.width(), y0 = height() - imageToShow.height();
	mousePosMutex.lock();
	mousePos = click_pos - ScreenVector(x0, y0) * 0.5;
	mousePosMutex.unlock();
}

void MapWidget::takeScreenShot()
{
	QDate d = QDate::currentDate();
	QTime t = QTime::currentTime();
	char buffer[64];
	sprintf_s(buffer, u8"游戏截图%04d%02d%02d%02d%02d%02d%03d.jpg", d.year(), d.month(), d.day(), t.hour(), t.minute(), t.second() ,t.msec());
	imageToShow.save(buffer);
}
