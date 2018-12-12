#include "stdafx.h"
#include "Obstacle.h"
#include "Bullet.h"

QImage ObstacleView::box_picture;

void ObstacleView::LoadResource()
{
	QString source_file_name = "../images/box.png";
	box_picture.load(source_file_name);
	if (box_picture.isNull())
		throw QString(u8"无法打开文件%1!").arg(source_file_name);
}

void ObstacleView::readStream(QDataStream &spriteStateData)
{
	bool is_new;
	spriteStateData >> is_new;
	if (is_new)
		spriteStateData >> centerPos.x >> centerPos.y;
}

void ObstacleView::drawBody(QPainter &painter, const M2S_Transformation &f)
{
	painter.setPen(QColor(0xff, 0x99, 0x33));
	painter.setBrush(QColor(0xff, 0x99, 0x33));
	ScreenVector wh = f(MapVector(100, -100));
	ScreenPoint p = f(centerPos);
	painter.drawImage(QRect(p.x - wh.x / 2, p.y - wh.y / 2, wh.x, wh.y), box_picture);
}

void ObstacleModel::writeStream(QDataStream &spriteStateData) const
{
	spriteStateData << isNew;
	if (isNew)
		spriteStateData << centerPos.x << centerPos.y;
}

bool ObstacleModel::collisionDetection(SpriteModel &other, double &delta_t)
{
	return other.staticCollisionDetection(collider, centerPos, delta_t);
}

bool ObstacleModel::staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t)
{
	return other.staticCollisionDetection(collider, centerPos, p2);
}

bool ObstacleModel::dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, p2, MapVector::Zeros(), v2, delta_t);
}

void ObstacleModel::dealCollision(SpriteModel &other, double t)
{
	switch (other.getType())
	{
	case SPRITE_BULLET:
	{
		// 障碍物中弹掉血，没血了会消失
		BulletModel &bullet = *static_cast<BulletModel*>(&other);
		toDelete = healthData.injured(bullet.damage);
		break;
	}
	}
}
