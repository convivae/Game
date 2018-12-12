#include "stdafx.h"
#include "DataManager.h"
#include "Actor.h"
#include "Bullet.h"
#include "Obstacle.h"
#include "Enemy.h"

void DataSender::EncodeSpriteState(QByteArray &dst, const QList<SpriteModel*> &src)
{
	dst.clear();
	QDataStream stream(&dst, QIODevice::WriteOnly);
	for each (const SpriteModel *p in src)
	{
		stream << p->toDelete;
		if (!p->toDelete)
			p->writeStream(stream);
	}		
}

void DataSender::EncodeNewSpriteInfo(QByteArray &dst, const QList<SpriteModel*> &src)
{
	dst.clear();
	QDataStream stream(&dst, QIODevice::WriteOnly);
	for each (SpriteModel *p in src)
	{
		stream << (quint8)p->getType();
		p->writeStream(stream);
		p->isNew = false;
	}	
}

void DataReceiver::DecodeSpriteState(QList<SpriteView*> &dst, QByteArray &src)
{
	QDataStream stream(&src, QIODevice::ReadOnly);
	for (auto it = dst.begin(); it != dst.end(); )
	{
		bool to_delete;
		stream >> to_delete;
		if (to_delete)
		{
			delete *it;
			it = dst.erase(it);
		}
		else
		{
			(*it)->readStream(stream);
			++it;
		}
	}
}

void DataReceiver::DecodeNewSpriteInfo(QList<SpriteView*> &dst, QByteArray &src)
{
	QDataStream stream(&src, QIODevice::ReadOnly);
	while (!stream.atEnd())
	{
		SpriteType type = SPRITE_ACTOR;
		stream >> *(quint8*)&type;
		switch (type)
		{
		case SPRITE_ACTOR:
			dst.append(new ActorView);
			break;
		case SPRITE_BULLET:
			dst.append(new BulletView);
			break;
		case SPRITE_OBSTACLE:
			dst.append(new ObstacleView);
			break;
		case SPRITE_ENEMY:
			dst.append(new EnemyView);
			break;
		default:
			throw "没有这种类型的精灵";
		}
		dst.back()->readStream(stream);
	}	
}

void DataManager::sendKeyState(const KeyState &state)
{
	inputStateMutex.lock();
	inputStateData.key = state;
	inputStateMutex.unlock();
}

void DataManager::sendMousePressInfo(const MapPoint &click_pos)
{
	inputStateMutex.lock();
	inputStateData.mouse.x = click_pos.x;
	inputStateData.mouse.y = click_pos.y;
	inputStateData.mouse.pressed = true;
	inputStateMutex.unlock();
}

void DataManager::sendMouseReleaseInfo()
{
	inputStateData.mouse.pressed = false;
}

void DataManager::sendSpriteState(const QList<SpriteModel*> &sprites)
{
	QByteArray tmp;
	EncodeSpriteState(tmp, sprites);
	sendSpriteState(tmp);
}

void DataManager::sendSpriteState(QByteArray &data)
{
	spriteStateMutex.lock();
	spriteStateData.push_back(data);
	spriteStateMutex.unlock();
}

void DataManager::sendNewSpriteInfo(const QList<SpriteModel*> &sprites)
{
	QByteArray tmp;
	EncodeNewSpriteInfo(tmp, sprites);
	sendNewSpriteInfo(tmp);
}

void DataManager::sendNewSpriteInfo(QByteArray &data)
{
	newSpriteInfoMutex.lock();
	newSpriteInfoData.push_back(data);
	newSpriteInfoMutex.unlock();
}

void DataManager::receiveInputState(InputState &state)
{
	inputStateMutex.lock();
	state = inputStateData;
	inputStateMutex.unlock();
}

void DataManager::receiveInputState(QVector<InputState> &states)
{
	receiveInputState(states[0]);
}

void DataManager::receiveSpriteState(QList<SpriteView*> &sprites)
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

void DataBroadcaster::sendSpriteState(const QList<SpriteModel*> &sprites)
{
	QByteArray tmp;
	EncodeSpriteState(tmp, sprites);
	sendSpriteState(tmp);
}

void DataBroadcaster::sendSpriteState(QByteArray &data)
{
	for each (DataSender *p in senders)
		p->sendSpriteState(data);
}

void DataBroadcaster::sendNewSpriteInfo(const QList<SpriteModel*> &sprites)
{
	QByteArray tmp;
	EncodeNewSpriteInfo(tmp, sprites);
	sendNewSpriteInfo(tmp);
}

void DataBroadcaster::sendNewSpriteInfo(QByteArray &data)
{
	for each (DataSender *p in senders)
		p->sendNewSpriteInfo(data);
}

void DataCollector::receiveInputState(QVector<InputState> &states)
{
	int i = 0;
	for each (DataReceiver *p in receivers)
		p->receiveInputState(states[i++]);
}
