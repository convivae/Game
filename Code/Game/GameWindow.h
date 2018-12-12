#pragma once

#include "ui_GameWindow.h"

#define INDEX_CHOOSE_WINDOW 0
#define INDEX_MAP_WIDGET 1

class PhysicsEngine;
class GameDisplayer;
class DataSender;
class DataReceiver;
class DataManager;
class DataBroadcaster;
class DataCollector;
class TcpSender;
class TcpReceiver;

class GameWindow : public QWidget
{
	Q_OBJECT

public:
	explicit GameWindow(QWidget *parent = nullptr);
	~GameWindow();

	void setStackWidgetIndex(int i);
	void singlePlayerGame();
	void multiplePlayerGame();
	void resetGame();

protected:
	void closeEvent(QCloseEvent *evt) override;
	void keyPressEvent(QKeyEvent *evt) override;

private:
	void multiplePlayerGameHost(QTcpServer *server, QSet<QTcpSocket*> &clients_ptr);
	void multiplePlayerGameClient(QTcpSocket *socket);
	void startGame(QString window_title, DataSender *sender, DataReceiver *receiver);

private:
	Ui::GameWindowClass ui;

	DataManager *dataManager;
	DataBroadcaster *broadcaster;
	DataCollector *collector;
	QVector<TcpSender*> tcpSenders;
	QVector<TcpReceiver*> tcpReceivers;
};
