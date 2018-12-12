#include "stdafx.h"
#include "MapModel.h"

WallModel::WallModel(const MapPoint &pos, double width, double height)
	: SpriteModel(SPRITE_WALL, 0), collider(width, height)
{
	centerPos = pos;
}

bool WallModel::collisionDetection(SpriteModel &other, double &delta_t)
{
	return other.staticCollisionDetection(collider, centerPos, delta_t);
}

bool WallModel::staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t)
{
	return other.staticCollisionDetection(collider, centerPos, p2);
}

bool WallModel::dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, p2, MapVector::Zeros(), v2, delta_t);
}

MapModel::MapModel(QString source_file_name, double block_size)
	: blockSize(block_size)
{
	// 获取地图信息
	QFile file(source_file_name);
	file.open(QIODevice::ReadOnly);
	if (!file.isOpen())
		throw QString(u8"无法打开文件%1!").arg(source_file_name);

	QVector<bool> data;
	QDataStream(&file) >> rows >> columns >> data;
	width = columns * blockSize;
	height = rows * blockSize;

	// 给不能通过的格子加碰撞器，使用贪心算法，尽量少加碰撞器
	data.append(false); // 防溢出
	for (int i = 0; i < rows; i++)
	{
		bool is_wall = false;
		int j0 = 0;
		for (int j = 0; j < columns; j++)
		{
			if (is_wall)
			{
				if (!data[i * columns + j])
				{
					int k = i + 1;
					memset(&data[i * columns + j0], 0, j - j0);
					while (k < rows && std::find(&data[k * columns + j0], &data[k * columns + j], false) == &data[k * columns + j])
					{
						memset(&data[k * columns + j0], 0, j - j0);
						k++;
					}
					addWall(i, j0, k - i, j - j0);
					is_wall = false;
				}
			}
			else
			{
				if (data[i * columns + j])
				{
					is_wall = true;
					j0 = j;
				}
			}
		}
		if (is_wall)
		{
			int k = i + 1;
			memset(&data[i * columns + j0], 0, columns - j0);
			while (k < rows && std::find(&data[k * columns + j0], &data[(k + 1) * columns], false) == &data[(k + 1) * columns])
			{
				memset(&data[k * columns + j0], 0, columns - j0);
				k++;
			}
			addWall(i, j0, k - i, columns - j0);
		}
	}
}

MapModel::~MapModel()
{
	for each (WallModel *p in walls)
		delete p;
}

void MapModel::addWall(int i, int j, int r, int c)
{
	walls.append(new WallModel(MapPoint((j + c * 0.5) * blockSize, -(i + r * 0.5) * blockSize), c * blockSize, r * blockSize));
}
