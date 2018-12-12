#include "stdafx.h"
#include "TcpSender.h"

TcpSender::TcpSender(QTcpSocket *socket, QObject *parent)
	: QObject(parent), socket(socket)
{
	connect(this, &TcpSender::sendSpriteState_private_signal, this, &TcpSender::sendSpriteState_private_slot);
	connect(this, &TcpSender::sendNewSpriteInfo_private_signal, this, &TcpSender::sendNewSpriteInfo_private_slot);
}

void TcpSender::sendKeyState(const KeyState &state)
{
	QDataStream(socket) << KEY_STATE << state.x << state.y;
}

void TcpSender::sendMousePressInfo(const MapPoint &click_pos)
{
	QDataStream(socket) << MOUSE_PRESS << click_pos.x << click_pos.y;
}

void TcpSender::sendMouseReleaseInfo()
{
	QDataStream(socket) << MOUSE_RELEASE;
}

void TcpSender::sendSpriteState(const QList<SpriteModel*> &sprites)
{
	QByteArray spriteStateData;
	EncodeSpriteState(spriteStateData, sprites);
	sendSpriteState(spriteStateData);
}

void TcpSender::sendSpriteState(QByteArray &data)
{
	emit sendSpriteState_private_signal(data);
}

void TcpSender::sendNewSpriteInfo(const QList<SpriteModel*>& sprites)
{
	QByteArray newSpriteInfoData;
	EncodeNewSpriteInfo(newSpriteInfoData, sprites);
	sendNewSpriteInfo(newSpriteInfoData);
}

void TcpSender::sendNewSpriteInfo(QByteArray &data)
{
	emit sendNewSpriteInfo_private_signal(data);
}

void TcpSender::sendSpriteState_private_slot(QByteArray data)
{
	QDataStream(socket) << UPDATE_SPRITE << data;
}

void TcpSender::sendNewSpriteInfo_private_slot(QByteArray data)
{
	QDataStream(socket) << NEW_SPRITE << data;
}
