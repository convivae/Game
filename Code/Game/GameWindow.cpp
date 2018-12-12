#include "stdafx.h"
#include "global_vars.h"
#include "GameWindow.h"
#include "GameController.h"
#include "PhysicsEngine.h"
#include "GameDisplayer.h"
#include "TcpSender.h"
#include "TcpReceiver.h"
#include "MapView.h"
#include "Actor.h"
#include "Enemy.h"
#include "Obstacle.h"

GameWindow::GameWindow(QWidget *parent)
	: QWidget(parent), dataManager(nullptr), broadcaster(nullptr), collector(nullptr)
{
	ui.setupUi(this);
	connect(ui.chooseWidget, &ChooseWidget::quitGameClicked, this, &QWidget::close);

	try
	{
		MapWidget::LoadResource();
		MapView::LoadResource();
		ActorView::LoadResource();
		EnemyView::LoadResource();
		ObstacleView::LoadResource();
	}
	catch (QString err_str)
	{
		QMessageBox::warning(this, u8"错误！", err_str);
		exit(0);
	}

	gameWindow = this;
	gameController = new GameController;
}

GameWindow::~GameWindow()
{
	resetGame();
	delete gameController;
}

void GameWindow::setStackWidgetIndex(int i)
{
	ui.stackedWidget->setCurrentIndex(i);
}

void GameWindow::singlePlayerGame()
{
	try
	{
		playersNum = 1;
		myID = 0;
		dataManager = new DataManager;
		physicsEngine = new PhysicsEngine(dataManager, dataManager);
		startGame(u8"单人游戏", dataManager, dataManager);
	}
	catch (QString err_str)
	{
		QMessageBox::warning(this, u8"错误！", err_str);
		exit(0);
	}
}

void GameWindow::multiplePlayerGame()
{
	try
	{
		if (ui.chooseWidget->server != nullptr)
			multiplePlayerGameHost(ui.chooseWidget->server, ui.chooseWidget->sockets);
		else
			multiplePlayerGameClient(*ui.chooseWidget->sockets.begin());
	}
	catch (QString err_str)
	{
		QMessageBox::warning(this, u8"错误！", err_str);
		exit(0);
	}
}

void GameWindow::resetGame()
{
	ui.mapWidget->resetGame();
	ui.chooseWidget->resetGame();
	if (physicsEngine != nullptr)
	{
		delete physicsEngine;
		physicsEngine = nullptr;
	}
	if (gameDisplayer != nullptr)
	{
		delete gameDisplayer;
		gameDisplayer = nullptr;
	}
	if (dataManager != nullptr)
	{
		delete dataManager;
		dataManager = nullptr;
	}
	if (broadcaster != nullptr)
	{
		delete broadcaster;
		broadcaster = nullptr;
	}
	if (collector != nullptr)
	{
		delete collector;
		collector = nullptr;
	}
	for each (TcpSender* p in tcpSenders)
		delete p;
	tcpSenders.clear();
	for each (TcpReceiver* p in tcpReceivers)
		delete p;
	tcpReceivers.clear();
}

void GameWindow::closeEvent(QCloseEvent *evt)
{
	gameController->stop();
	evt->accept();
}

void GameWindow::keyPressEvent(QKeyEvent *evt)
{
	switch (evt->key())
	{
	case Qt::Key_F11:
		if (isFullScreen())
			showNormal();
		else
			showFullScreen();
		return evt->accept();
	default:
		return evt->ignore();
	}
}

void GameWindow::multiplePlayerGameHost(QTcpServer *server, QSet<QTcpSocket*> &clients)
{
	dataManager = new DataManager;
	QVector<DataSender*> senders;
	QVector<DataReceiver*> receivers;
	senders.append(dataManager);
	receivers.append(dataManager);

	for each (QTcpSocket *p in clients)
	{
		tcpSenders.append(new TcpSender(p));
		senders.append(tcpSenders.back());
		tcpReceivers.append(new TcpReceiver(p));
		receivers.append(tcpReceivers.back());
	}

	broadcaster = new DataBroadcaster(senders);
	collector = new DataCollector(receivers);
	physicsEngine = new PhysicsEngine(broadcaster, collector);
	startGame(u8"多人游戏-主场", dataManager, dataManager);
}

void GameWindow::multiplePlayerGameClient(QTcpSocket *socket)
{
	tcpSenders.append(new TcpSender(socket));
	tcpReceivers.append(new TcpReceiver(socket));
	connect(socket, &QTcpSocket::disconnected, ui.mapWidget, &MapWidget::hostQuit);
	startGame(QString(u8"多人游戏-客场%1号位").arg(myID), tcpSenders[0], tcpReceivers[0]);
}

void GameWindow::startGame(QString window_title, DataSender *sender, DataReceiver *receiver)
{
	ui.mapWidget->setDataSender(sender);
	gameDisplayer = new GameDisplayer(receiver);
	setWindowTitle(window_title);
	setStackWidgetIndex(INDEX_MAP_WIDGET);
	ui.mapWidget->setFocus();
}
