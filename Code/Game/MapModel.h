#pragma once

#include "Sprite.h"

// 墙类，用来做碰撞检测的
class WallModel : public SpriteModel
{
public:
	WallModel(const MapPoint &pos, double width, double height);

	bool collisionDetection(SpriteModel &other, double &delta_t) override;
	bool staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t) override;
	bool dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t) override;

	double maxX() const override { return centerPos.x + collider.halfWH.x; }
	double minX() const override { return centerPos.x - collider.halfWH.x; }
	double maxY() const override { return centerPos.y + collider.halfWH.y; }
	double minY() const override { return centerPos.y - collider.halfWH.y; }

private:
	RectShapedCollider collider;
};

// 地图模型类，用于判定地图上各地点是否可通行
// 在地图坐标系中，地图的左上角是(0, 0), 右下角是(width(), -height())
class MapModel
{
public:
	MapModel(QString source_file_name, double block_size);
	~MapModel();

	double getWidth() const { return width; }
	double getHeight() const { return height; }
	QVector<SpriteModel*> getWalls() const { return walls; }

private:
	void addWall(int i, int j, int r, int c);

private:
	int rows, columns;
	double width, height;
	QVector<SpriteModel*> walls;
	double blockSize;
};

