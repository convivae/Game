#pragma once

#include "DataManager.h"

class TcpSender : public QObject, public DataSender
{
	Q_OBJECT

public:
	TcpSender(QTcpSocket *socket, QObject *parent = nullptr);

	void sendKeyState(const KeyState &data) override;
	void sendMousePressInfo(const MapPoint &click_pos) override;
	void sendMouseReleaseInfo() override;
	void sendSpriteState(const QList<SpriteModel*> &data) override;
	void sendSpriteState(QByteArray &data) override;
	void sendNewSpriteInfo(const QList<SpriteModel*> &sprites) override;
	void sendNewSpriteInfo(QByteArray &data) override;

signals:
	void sendSpriteState_private_signal(QByteArray data);
	void sendNewSpriteInfo_private_signal(QByteArray data);

private:
	void sendSpriteState_private_slot(QByteArray data);
	void sendNewSpriteInfo_private_slot(QByteArray data);

private:
	QTcpSocket *socket;
	QByteArray spriteStateData, newSpriteInfoData;
};
