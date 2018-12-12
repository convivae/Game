#include "stdafx.h"
#include "TcpReceiver.h"

TcpReceiver::TcpReceiver(QTcpSocket *socket, QObject *parent)
	: QObject(parent), socket(socket), state(0)
{
	connect(socket, &QTcpSocket::readyRead, this, &TcpReceiver::storeData);
}

void TcpReceiver::receiveInputState(InputState &state)
{
	inputStateMutex.lock();
	state = inputStateData;
	inputStateMutex.unlock();
}

void TcpReceiver::receiveSpriteState(QList<SpriteView*> &sprites)
{
	spriteStateMutex.lock();
	newSpriteInfoMutex.lock();
	int n = spriteStateData.size() < newSpriteInfoData.size() ? spriteStateData.size() : newSpriteInfoData.size();
	n = (n + 1) / 2;
	for (int i = 0; i < n; i++)
	{
		DecodeSpriteState(sprites, spriteStateData.front());
		spriteStateData.pop_front();
		DecodeNewSpriteInfo(sprites, newSpriteInfoData.front());
		newSpriteInfoData.pop_front();
	}
	newSpriteInfoMutex.unlock();
	spriteStateMutex.unlock();
}

void TcpReceiver::storeData()
{
	QDataStream stream(socket);
	while (true)
	{
		switch (state)
		{
		case 0: // 当前没有未读完的报文
		{
			if (socket->bytesAvailable() >= 1)
			{
				stream >> dataType;
				switch (dataType)
				{
				case KEY_STATE:
					state = 1;
					continue;
				case MOUSE_PRESS:
					state = 2;
					continue;
				case MOUSE_RELEASE:
					inputStateData.mouse.pressed = false;
					state = 0;
					continue;
				case UPDATE_SPRITE:
					state = 3;
					continue;
				case NEW_SPRITE:
					state = 4;
					continue;
				default:
					throw "错误";
				}
			}
			return;
		}
		case 1:
		{
			if (socket->bytesAvailable() >= 2)
			{
				inputStateMutex.lock();
				stream >> inputStateData.key.x >> inputStateData.key.y;
				inputStateMutex.unlock();
				state = 0;
				continue;
			}
			return;
		}
		case 2:
		{
			if (socket->bytesAvailable() >= 16)
			{
				inputStateMutex.lock();
				stream >> inputStateData.mouse.x >> inputStateData.mouse.y;
				inputStateData.mouse.pressed = true;
				inputStateMutex.unlock();
				state = 0;
				continue;
			}
			return;
		}
		case 3:
		{
			if (socket->bytesAvailable() >= 4)
			{
				stream >> n1;
				n1 = n1 > 0 ? n1 : 0;
				state = 5;
				continue;
			}
			return;
		}
		case 4:
		{
			if (socket->bytesAvailable() >= 4)
			{
				stream >> n2;
				n2 = n2 > 0 ? n2 : 0;
				state = 6;
				continue;
			}
			return;
		}
		case 5:
		{
			if (socket->bytesAvailable() >= n1)
			{
				spriteStateMutex.lock();
				spriteStateData.push_back(socket->read(n1));
				spriteStateMutex.unlock();
				state = 0;
				continue;
			}
			return;
		}
		case 6:
		{
			if (socket->bytesAvailable() >= n2)
			{
				newSpriteInfoMutex.lock();
				newSpriteInfoData.push_back(socket->read(n2));
				newSpriteInfoMutex.unlock();
				state = 0;
				continue;
			}
			return;
		}
		default:
			throw "错误";
		}		
	}
}
