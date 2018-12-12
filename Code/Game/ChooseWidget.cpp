#include "stdafx.h"
#include "global_vars.h"
#include "ChooseWidget.h"

#define MY_PORT 8888

ChooseWidget::ChooseWidget(QWidget *parent)
	: QWidget(parent), server(nullptr)
{
	ui.setupUi(this);
	chooseWidget = this;
	
	connect(ui.pushButton_single, &QPushButton::clicked, this, &ChooseWidget::singlePlayerClicked);
	connect(ui.pushButton_multiple, &QPushButton::clicked, this, &ChooseWidget::multiplePlayerClicked);
	connect(ui.pushButton_quit, &QPushButton::clicked, this, &ChooseWidget::quitGameClicked);

	connect(ui.pushButton_create, &QPushButton::clicked, this, &ChooseWidget::createRoomClicked);
	connect(ui.pushButton_join, &QPushButton::clicked, this, &ChooseWidget::joinRoomClicked);
	connect(ui.pushButton_back0, &QPushButton::clicked, this, &ChooseWidget::backToMainMenuClicked);

	connect(ui.pushButton_closeRoom, &QPushButton::clicked, this, &ChooseWidget::closeRoomClicked);
	connect(ui.pushButton_hostStart, &QPushButton::clicked, this, &ChooseWidget::hostStartClicked);

	connect(ui.pushButton_connect, &QPushButton::clicked, this, &ChooseWidget::connectIPClicked);
	connect(ui.pushButton_back1, &QPushButton::clicked, this, &ChooseWidget::cancelSeekingClicked);

	connect(ui.pushButton_back3, &QPushButton::clicked, this, &ChooseWidget::leaveRoomClicked);

	connect(&timer, &QTimer::timeout, this, &ChooseWidget::connectFailed);
	timer.setSingleShot(true);
}

ChooseWidget::~ChooseWidget()
{
	resetGame();
}

void ChooseWidget::setStackWidgetIndex(int i)
{
	ui.stackedWidget->setCurrentIndex(i);
}

void ChooseWidget::createRoom()
{
	server = new QTcpServer(this);
	connect(server, &QTcpServer::newConnection, this, &ChooseWidget::newClientConnected);
	server->listen(QHostAddress::AnyIPv4, MY_PORT);
}

void ChooseWidget::closeRoom()
{
	delete server;
	server = nullptr;
	sockets.clear();
}

void ChooseWidget::readyToInputIP()
{
	ui.pushButton_connect->setEnabled(true);
	ui.pushButton_back1->setText(u8"返回");
}

void ChooseWidget::seekRoom()
{
	sockets.insert(new QTcpSocket(this));
	connect(*sockets.begin(), &QTcpSocket::connected, this, &ChooseWidget::connectSucceeded);
	connect(*sockets.begin(), &QTcpSocket::readyRead, this, &ChooseWidget::hostStart);
	connect(*sockets.begin(), &QTcpSocket::disconnected, this, &ChooseWidget::hostLeave);
	(*sockets.begin())->connectToHost(QHostAddress(ui.lineEdit_ip->text()), MY_PORT);
	timer.start(10000);
	ui.pushButton_connect->setEnabled(false);
	ui.pushButton_back1->setText(u8"取消");
}

void ChooseWidget::cancelSeeking()
{
	delete *sockets.begin();
	sockets.clear();
}

void ChooseWidget::notFoundRoom()
{
	cancelSeeking();
	QMessageBox::information(this, u8"消息", u8"连接超时");
}

void ChooseWidget::joinedRoom()
{
	timer.stop();
}

void ChooseWidget::leaveRoomHost()
{
	(*sockets.begin())->deleteLater();
	QMessageBox::information(chooseWidget, u8"消息", u8"房主关闭了房间");
	sockets.clear();
	ui.pushButton_connect->setEnabled(true);
}

void ChooseWidget::leaveRoomClient()
{
	delete *sockets.begin();
	sockets.clear();
	ui.pushButton_connect->setEnabled(true);
}

void ChooseWidget::startGameServer()
{
	server->close();
	playersNum = sockets.size() + 1;
	myID = 0;
	int client_id = 0;
	for each (QTcpSocket *p in sockets)
		QDataStream(p) << playersNum << ++client_id;
	_sleep(10);
}

void ChooseWidget::startGameClient()
{
	disconnect(*sockets.begin());
	QDataStream(*sockets.begin()) >> playersNum >> myID;
}

void ChooseWidget::resetGame()
{
	if (server != nullptr)
	{
		delete server;
		server = nullptr;
	}
	else if (!sockets.empty())
	{
		(*sockets.begin())->deleteLater();
	}
	sockets.clear();
}

void ChooseWidget::newClientConnected()
{
	while (server->hasPendingConnections())
	{
		QTcpSocket *socket = server->nextPendingConnection();
		sockets.insert(socket);
		connect(socket, &QTcpSocket::disconnected, [=]() 
		{ 
			sockets.remove(socket); 
			showConnection();
		});
	}
	showConnection();
}

void ChooseWidget::showConnection()
{
	QString s(u8"已连接：\n");
	if (sockets.empty())
	{
		s += u8"无\n";
	}
	else
	{
		for each (QTcpSocket *p in sockets)
			s += QString("%1\n").arg(p->peerAddress().toString());
	}
	ui.label_Players->setText(s);
}
