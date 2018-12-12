#pragma once

#include "Sprite.h"

class BulletView : public SpriteView
{
public:
	void readStream(QDataStream &spriteStateData) override;
	void drawBody(QPainter &painter, const M2S_Transformation &f) override;

private:
	double radius;
};

class BulletModel : public SpriteModel
{
public:
	BulletModel(bool is_enemy, int damage, double radius, double speed)
		: SpriteModel(SPRITE_BULLET, is_enemy ? 5 : 4), damage(damage), collider(radius), speed(speed) {}

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

	void setDirection(const MapVector &direction);

public:
	int damage;

protected:
	CircleShapedCollider collider;
	MapVector velocity;
	double speed;
};
