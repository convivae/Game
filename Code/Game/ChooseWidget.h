#pragma once

#include "ui_ChooseWidget.h"

#define INDEX_MAIN_MENU 0
#define INDEX_CREATE_JOIN 1
#define INDEX_HOST_WAIT 2
#define INDEX_CLIENT_CONNECT 3
#define INDEX_CLIENT_WAIT 4

class ChooseWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ChooseWidget(QWidget *parent = nullptr);
	~ChooseWidget();

	void setStackWidgetIndex(int i);
	void createRoom();
	void closeRoom();
	void readyToInputIP();
	void seekRoom();
	void cancelSeeking();
	void notFoundRoom();
	void joinedRoom();
	void leaveRoomHost();
	void leaveRoomClient();
	void startGameServer();
	void startGameClient();
	void resetGame();

signals:
	void singlePlayerClicked();
	void multiplePlayerClicked();
	void quitGameClicked();
	void createRoomClicked();
	void joinRoomClicked();
	void backToMainMenuClicked();
	void closeRoomClicked();
	void hostStartClicked();
	void connectIPClicked();
	void backCreateOrJoinClicked();
	void cancelSeekingClicked();
	void leaveRoomClicked();
	void connectSucceeded();
	void connectFailed();
	void hostStart();
	void hostLeave();

private:
	void newClientConnected();
	void showConnection();

public:
	QTcpServer *server;
	QSet<QTcpSocket*> sockets;

private:
	Ui::ChooseWidget ui;
	QTimer timer;
	friend class GameController;
};
