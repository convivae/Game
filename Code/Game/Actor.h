#pragma once

#include "Sprite.h"

class InputState;

class ActorView : public SpriteView
{
public:
	ActorView();
	~ActorView();

	static void LoadResource();

	void readStream(QDataStream &spriteStateData) override;
	void drawBody(QPainter &painter, const M2S_Transformation &f) override;
	void drawBloodBar(QPainter &painter, const M2S_Transformation &f) override;
	void drawBloodBar(QPainter &painter, int x, int y, int w, int h);

public:
	static ActorView *myActor;

private:
	static QVector<QImage> actor_pictures[2];

	int health, maxHealth, shield, maxShield, magic, maxMagic;
	MapPoint actorLastPos;
	double radius;
	double last_vx;
	int id, pictureIndex;
};

class ActorModel : public SpriteModel
{
public:
	ActorModel(const InputState *ctrlInfo, MapPoint *posInfo, int id, int pictureIndex, 
		int health, int shield, int magic, double radius, double speed)
		: SpriteModel(SPRITE_ACTOR, 1), ctrlInfo(ctrlInfo), posInfo(posInfo)
		, id(id), pictureIndex(pictureIndex)
		, healthData(health, shield), magicData(magic) 
		, collider(radius), collisionTime(INFINITY), collised(false)
		, speed(speed) {}

	void writeStream(QDataStream &spriteStateData) const override;
	void action() override;
	bool collisionDetection(SpriteModel &other, double &delta_t) override;
	bool staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t) override;
	bool dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t) override;
	void dealCollision(SpriteModel &other, double t) override;
	void updatePos(double delta_t) override;
	void borderCheck(double map_width, double map_height) override;
	void updateTime(double delta_t) override;

	double maxX() const override { return centerPos.x + collider.radius + fabs(velocity.x); }
	double minX() const override { return centerPos.x - collider.radius - fabs(velocity.x); }
	double maxY() const override { return centerPos.y + collider.radius + fabs(velocity.y); }
	double minY() const override { return centerPos.y - collider.radius - fabs(velocity.y); }

public:
	HealthDataWithShield healthData;
	MagicData magicData;

protected:
	const InputState *ctrlInfo;
	MapPoint *posInfo;

	CooldownData<1> cooldownWeapon; // 使用武器的冷却时间
	CooldownData<1> cooldownDamage; // 连续伤害的冷却时间
	CooldownData<2> cooldownShield; // 护盾回复的冷却时间，分别是没挨打和挨打了的数据

	CircleShapedCollider collider;
	MapVector velocity;
	double speed;
	double collisionTime;
	bool collised;

private:
	int id, pictureIndex;
	friend class EnemyModel;
};

class ActorMagicianModel : public ActorModel
{
public:
	ActorMagicianModel(InputState *ctrlInfo, MapPoint *posInfo, int id);
};
