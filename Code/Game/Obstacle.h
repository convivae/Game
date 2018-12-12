#pragma once

#include "Sprite.h"

class ObstacleView : public SpriteView
{
public:
	static void LoadResource();

	void readStream(QDataStream &spriteStateData) override;
	void drawBody(QPainter &painter, const M2S_Transformation &f) override;

private:
	static QImage box_picture;
};

class ObstacleModel : public SpriteModel
{
public:
	ObstacleModel(int health, double length)
		: SpriteModel(SPRITE_OBSTACLE, 3), healthData(health), collider(length, length) {}

	void writeStream(QDataStream &spriteStateData) const override;
	bool collisionDetection(SpriteModel &other, double &delta_t) override;
	bool staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t) override;
	bool dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t) override;
	void dealCollision(SpriteModel &other, double t) override;

	double maxX() const override { return centerPos.x + collider.halfWH.x; }
	double minX() const override { return centerPos.x - collider.halfWH.x; }
	double maxY() const override { return centerPos.y + collider.halfWH.y; }
	double minY() const override { return centerPos.y - collider.halfWH.y; }

public:
	HealthData healthData;

protected:
	RectShapedCollider collider;
};
