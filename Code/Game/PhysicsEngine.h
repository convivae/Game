#pragma once

#include "WorkerThread.h"
#include "DataManager.h"
#include "Sprite.h"

//#define COLLISION_COUNT

enum BarrageType
{
	ACTOR_BULLET, ENEMY_BULLET_SMALL, ENEMY_BULLET_BIG
};

class MapModel;

class PhysicsEngine : public WorkerThread
{
	Q_OBJECT

public:
	PhysicsEngine(DataSender *sender, DataReceiver *receiver);
	~PhysicsEngine() override;

	void addBarrage(const MapPoint &start_pos, const MapVector &direction, BarrageType type);
	void addSprite(SpriteModel *p);
	QVector<MapPoint> getActorsPos() const { return actorsPos; }
	MapPoint getNearestActorPos(const MapPoint &enemyPos) const;

protected:
	void mainloop() override;

private:
	void collisionDecide(QVector<SpriteModel*> &src);
	void collisionDecide(QVector<SpriteModel*> &src1, QVector<SpriteModel*> &src2, bool i_j, bool j_i);
	void collisionDecide(QVector<SpriteModel*> &src, const ClosedRegion2D &region, bool vertical);
	void collisionDecide(QVector<SpriteModel*> &src1, QVector<SpriteModel*> &src2, bool i_j, bool j_i, const ClosedRegion2D &region, bool vertical);
	void verticalDivide(const QVector<SpriteModel*> &src, double x, QVector<SpriteModel*> &left, QVector<SpriteModel*> &right, QVector<SpriteModel*> &cross);
	void horizontalDivide(const QVector<SpriteModel*> &src, double y, QVector<SpriteModel*> &up, QVector<SpriteModel*> &down, QVector<SpriteModel*> &cross);

public:
	bool pauseGame;

private:
	QList<SpriteModel*> allSprites, newSprites;
	DataSender *dataSender;
	DataReceiver *dataReceiver;
	QVector<InputState> inputState;
	QVector<MapPoint> actorsPos;
	MapModel *mapModel;

#ifdef COLLISION_COUNT
	int collisionDecideCount;
#endif
};
