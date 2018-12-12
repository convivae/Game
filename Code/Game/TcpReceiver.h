#pragma once

#include "DataManager.h"

class TcpReceiver : public QObject, public DataReceiver
{
	Q_OBJECT

public:
	TcpReceiver(QTcpSocket *socket, QObject *parent = nullptr);

	void receiveInputState(InputState &data) override;
	void receiveSpriteState(QList<SpriteView*> &data) override;

private:
	void storeData();

private:
	QTcpSocket *socket;
	QMutex inputStateMutex, spriteStateMutex, newSpriteInfoMutex;
	InputState inputStateData;
	QQueue<QByteArray> spriteStateData, newSpriteInfoData;
	int state;
	quint8 dataType;
	int n1, n2;
};
