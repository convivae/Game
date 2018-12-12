#pragma once

#include "Sprite.h"

class EnemyView : public SpriteView
{
public:
	EnemyView();
	~EnemyView();

	static void LoadResource();

	void readStream(QDataStream &spriteStateData) override;
	void drawBody(QPainter &painter, const M2S_Transformation &f) override;
	void drawBloodBar(QPainter &painter, const M2S_Transformation &f) override;

public:
	static int bossHealth, bossMaxHealth;

private:
	static QVector<QImage> monster_pictures[3];

	int health, maxHealth;
	int pictureIndex;
	bool isBoss;
	double radius;
	double last_vx;
	MapPoint monsterLastPos;
};

class EnemyModel : public SpriteModel
{
public:
	EnemyModel(int pictureIndex, bool isBoss,
		qint64 health, int damage, double radius, double speed)
		: SpriteModel(SPRITE_ENEMY, 2), pictureIndex(pictureIndex), isBoss(isBoss)
		, healthData(health), damage(damage)
		, collider(radius), collisionTime(INFINITY), collised(false)
		, speed(speed) {}

	void writeStream(QDataStream &spriteStateData) const override;
	bool collisionDetection(SpriteModel &other, double &delta_t) override;
	bool staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t) override;
	bool dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t) override;
	void dealCollision(SpriteModel &other, double t) override;
	void updatePos(double delta_t) override;
	void borderCheck(double map_width, double map_height) override;

	double maxX() const override { return centerPos.x + collider.radius + fabs(velocity.x); }
	double minX() const override { return centerPos.x - collider.radius - fabs(velocity.x); }
	double maxY() const override { return centerPos.y + collider.radius + fabs(velocity.y); }
	double minY() const override { return centerPos.y - collider.radius - fabs(velocity.y); }

public:
	int damage; // 碰撞伤害

protected:
	HealthData healthData;
	MapVector velocity;
	double speed;

	CircleShapedCollider collider;
	double collisionTime;
	bool collised;

private:
	int pictureIndex;
	bool isBoss;
	friend class ActorModel;
};

class EnemyPigModel : public EnemyModel
{
public:
	EnemyPigModel();

	void action() override;

	friend class ActorModel;
};

class EnemyMobModel : public EnemyModel
{
public:
	EnemyMobModel();

	void action() override;
	void updateTime(double delta_t) override;

private:
	CooldownData<1> cooldownWeapon; // 使用武器的冷却时间

	friend class ActorModel;
};

class EnemyBossModel : public EnemyModel
{
public:
	EnemyBossModel();

	void action() override;
	void updateTime(double delta_t) override;

private:
	CooldownData<1> cooldownWeapon; // 使用武器的冷却时间
	CooldownData<1> cooldownSkill[2]; // 使用技能的冷却时间

	friend class ActorModel;
};
