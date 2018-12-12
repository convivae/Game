#include "stdafx.h"
#include "Bullet.h"

void BulletView::readStream(QDataStream &spriteStateData)
{
	spriteStateData >> centerPos.x >> centerPos.y;
	bool is_new;
	spriteStateData >> is_new;
	if (is_new)
		spriteStateData >> radius;
}

void BulletView::drawBody(QPainter &painter, const M2S_Transformation &f)
{
	painter.setPen(QPen(Qt::red));
	painter.setBrush(QBrush(Qt::red));
	ScreenVector r_r = f(MapVector(radius, -radius));
	painter.drawEllipse(f(centerPos).toQPointF(), r_r.x, r_r.y);
}

void BulletModel::writeStream(QDataStream &spriteStateData) const
{
	spriteStateData << centerPos.x << centerPos.y;
	spriteStateData << isNew;
	if (isNew)
		spriteStateData << collider.radius;
}

bool BulletModel::collisionDetection(SpriteModel &other, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, velocity, delta_t);
}

bool BulletModel::staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, p2, velocity, MapVector::Zeros(), delta_t);
}

bool BulletModel::dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, p2, velocity, v2, delta_t);
}

void BulletModel::dealCollision(SpriteModel &other, double t)
{
	switch (other.getType())
	{
	case SPRITE_ACTOR:
	case SPRITE_OBSTACLE:
	case SPRITE_WALL:
	case SPRITE_ENEMY:
		toDelete = true;
		break;
	}
}

void BulletModel::updatePos(double delta_t)
{
	centerPos += velocity * delta_t;
}

void BulletModel::borderCheck(double map_width, double map_height)
{
	if (centerPos.x > map_width)
		toDelete = true;
	else if (centerPos.x < 0)
		toDelete = true;
	if (centerPos.y < -map_height)
		toDelete = true;
	else if (centerPos.y > 0)
		toDelete = true;
}

void BulletModel::setDirection(const MapVector &direction)
{
	velocity = direction.normalize() * speed;
}
