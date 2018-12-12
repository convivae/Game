#include "stdafx.h"
#include "global_vars.h"
#include "GameDisplayer.h"
#include "MapWidget.h"
#include "DataManager.h"
#include "MapView.h"
#include "Enemy.h"

#define ROWS_PER_SCREEN 13.5
#define COLUMNS_PER_SCREEN 24.0
#define MAP_BLOCK_SIZE 100.0

GameDisplayer::GameDisplayer(DataReceiver *receiver)
	: dataReceiver(receiver), s2mt({ 1, -1 }, { 0, 0 })
	, pauseGame(false), bitmapPainted(false), actorCreated(false)
{
	name = u8"绘图模块";

	connect(this, &GameDisplayer::finishedPainting, mapWidget, &MapWidget::updateImage);
	connect(this, &GameDisplayer::actorDied, mapWidget, &MapWidget::actorDied);

	mapView = new MapView("../levels/view/2");

	ActorView::myActor = nullptr;

	defaultFPS = 60;
	emit started();
}

GameDisplayer::~GameDisplayer()
{
	waitAndQuit();
	for each (SpriteView *p in sprites)
		delete p;
	gameDisplayer = nullptr;
}

S2M_Transformation GameDisplayer::getScreenToMapTrans()
{
	s2mtMutex.lock();
	S2M_Transformation tmp = s2mt;
	s2mtMutex.unlock();
	return tmp;
}

void GameDisplayer::mainloop()
{
	// 接收并更新精灵参数，同时添加新精灵和销毁无用精灵
	dataReceiver->receiveSpriteState(sprites);

	// 获取所属窗口大小
	double width = mapWidget->width(), height = mapWidget->height();

	// 如果暂停且已经画过，则不画
	if (pauseGame)
	{
		if (bitmapPainted && lastWidth == width && lastHeight == height)
			return;
		bitmapPainted = true;
	}
	bitmapPainted = pauseGame;
	lastWidth = width;
	lastHeight = height;

	// 如果主角没建立，则不画
	if (ActorView::myActor == nullptr)
	{
		// 如果主角死了，则发送信号
		if (actorCreated)
		{
			pauseGame = true;
			emit actorDied();
		}
		return;
	}		
	actorCreated = true;

	// 根据16:9的比例调整图片大小
	if (width * 9.0 > height * 16.0)
		width = height * (16.0 / 9.0);
	else if (width * 9.0 < height * 16.0)
		height = width * (9.0 / 16.0);
	
	// 计算坐标变换参数
	double block_size = width / COLUMNS_PER_SCREEN;
	double stretch_x = block_size / MAP_BLOCK_SIZE, stretch_y = -stretch_x;
	double max_screen_x = mapView->getColumns() * block_size, max_screen_y = mapView->getRows() * block_size;
	double bias_x, bias_y;

	if (width >= max_screen_x)
		bias_x = (width - max_screen_x) * 0.5;
	else if (ActorView::myActor->centerPos.x * stretch_x <= width * 0.5)
		bias_x = 0;
	else if (ActorView::myActor->centerPos.x * stretch_x >= max_screen_x - width * 0.5)
		bias_x = width - max_screen_x;
	else
		bias_x = width * 0.5 - ActorView::myActor->centerPos.x * stretch_x;

	if (height >= max_screen_y)
		bias_y = (height - max_screen_y) * 0.5;
	else if (ActorView::myActor->centerPos.y * stretch_y < height * 0.5)
		bias_y = 0;
	else if (ActorView::myActor->centerPos.y * stretch_y >= max_screen_y - height * 0.5)
		bias_y = height - max_screen_y;
	else
		bias_y = height * 0.5 - ActorView::myActor->centerPos.y * stretch_y;

	// 创建坐标变换对象
	M2S_Transformation m2st(stretch_x, stretch_y, bias_x, bias_y);
	setScreenToMapTrans(m2st.reverse());

	// 创建图像和画家对象
	imageMutex.lock();
	if (imageToShow.isNull() || imageToShow.width() != (int)width || imageToShow.height() != (int)height)
		imageToShow = QPixmap(width, height);
	imageToShow.fill(Qt::black);
	QPainter painter(&imageToShow);

	// 获取鼠标指针位置
	ScreenPoint mouse_pos = mapWidget->getMousePos();

	// 画地图
	mapView->drawBody(painter, m2st(MapPoint::Zeros()), block_size, width, height);

	// 画精灵
	for each (SpriteView *p in sprites)
		p->drawBody(painter, m2st);

	// 画血条
	for each (SpriteView *p in sprites)
		p->drawBloodBar(painter, m2st);
	ActorView::myActor->drawBloodBar(painter, width / 15, height / 15, width / 5, height / 5);
	SpriteView::DrawBloodBar(painter, QRect(width / 3, height / 50, width / 3, height / 30), EnemyView::bossHealth, EnemyView::bossMaxHealth, BOSS_BLOOD_BAR);

	// 绘制完成，通知所属窗口更新
	emit finishedPainting();
	imageMutex.unlock();
}

void GameDisplayer::setScreenToMapTrans(const S2M_Transformation &t)
{
	s2mtMutex.lock();
	s2mt = t;
	s2mtMutex.unlock();
}
