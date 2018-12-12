#pragma once

#include "Collider.h"

class PhysicsEngine;

// 用于区分子类具体类型的枚举，与子类一一对应
enum SpriteType
{
	SPRITE_ACTOR, SPRITE_BULLET, SPRITE_OBSTACLE, SPRITE_ENEMY, SPRITE_WALL
};

// 血条样式枚举
enum BloodBarType
{
	RED_BLOOD_BAR, BLUE_MAGIC_BAR, GRAY_SHELLED_BAR, BOSS_BLOOD_BAR, MOB_BLOOD_BAR
};

// 用于记录血量的结构体
struct HealthData
{
	HealthData(int health)
		: health(health), maxHealth(health) {}

	// 调用这个来造成伤害，如果死了返回true，没死返回false
	bool injured(int damage);
	
	int health, maxHealth;
};

// 血量结构体的带护甲版本
struct HealthDataWithShield : public HealthData
{
	HealthDataWithShield(int health, int shield)
		: HealthData(health), shield(shield), maxShield(shield) {}

	// 调用这个来造成伤害，先掉甲后掉血，其他同上
	bool injured(int damage);

	int shield, maxShield;
};

// 用于记录魔法值的结构体
struct MagicData
{
	MagicData(int magic)
		: magic(magic), maxMagic(magic) {}

	// 调用这个来消耗魔法，如果剩余魔法不够则返回false，消耗成功返回true
	bool consume(int cost);

	int magic, maxMagic;
};

// 用于记录技能冷却时间的结构体
template<size_t n>
struct CooldownData
{
	CooldownData() : remainTime(0), totalTime{0} {}

	double remainTime, totalTime[n];
};

// 精灵视图的基类，绘图模块用到
class SpriteView
{
public:
	virtual ~SpriteView() {}

protected:
	SpriteView() {}

public:
	static void DrawBloodBar(QPainter &painter, QRect rect, int health, int maxhp, BloodBarType type);
	static void DrawBloodBackground(QPainter &painter, QRect rect);

	virtual void readStream(QDataStream &spriteStateData) {}
	virtual void drawBody(QPainter &painter, const M2S_Transformation &f) {}
	virtual void drawBloodBar(QPainter &painter, const M2S_Transformation &f) {}

public:
	MapPoint centerPos;
};

// 精灵模型的基类，物理引擎模块用到
class SpriteModel
{
public:
	virtual ~SpriteModel() {}

protected:
	SpriteModel(SpriteType type, int kind) : type(type), kind(kind), toDelete(false), isNew(true) {}

public:
	static bool SketchyCollisionDetection(const SpriteModel &src1, const SpriteModel &src2);

	virtual void writeStream(QDataStream &spriteStateData) const {}
	virtual void action() {}
	virtual bool collisionDetection(SpriteModel &other, double &delta_t) { return false; }
	virtual bool staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t) { return false; }
	virtual bool dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t) { return false; }
	virtual void dealCollision(SpriteModel &other, double t) {}
	virtual void updatePos(double delta_t) {}
	virtual void borderCheck(double map_width, double map_height) {}
	virtual void updateTime(double delta_t) {}

	virtual double maxX() const = 0;
	virtual double minX() const = 0;
	virtual double maxY() const = 0;
	virtual double minY() const = 0;

	SpriteType getType() const { return type; }
	int getCollisionKind() const { return kind; }
	ClosedRegion2D wrappedRect() const { return ClosedRegion2D(minX(), maxX(), minY(), maxY()); }

	
public:
	MapPoint centerPos;
	bool toDelete, isNew;

private:
	SpriteType type;
	int kind;
};

