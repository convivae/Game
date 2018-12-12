#include "stdafx.h"
#include "global_vars.h"
#include "PhysicsEngine.h"
#include "DataManager.h"
#include "MapModel.h"
#include "Actor.h"
#include "Bullet.h"
#include "Obstacle.h"
#include "Enemy.h"

#define COLLISION_KIND_NUM 10

PhysicsEngine::PhysicsEngine(DataSender *sender, DataReceiver *receiver)
	: dataSender(sender), dataReceiver(receiver), pauseGame(false)
{
	name = u8"物理引擎";

	inputState.resize(playersNum);
	actorsPos.resize(playersNum);

	mapModel = new MapModel("../levels/model/2", 100);

	for (int i = 0; i < playersNum; i++)
		newSprites.append(new ActorMagicianModel(&inputState[i], &actorsPos[i], i));
	for (int i = 0; i < playersNum; i++)
		newSprites[i]->centerPos.setXY(8600 + (i % 8) * 150, -5000 - (i / 8) * 150);

	//右边房间
	for (int i = 4750; i <= 5150; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(8250, -i);
	}
	for (int i = 8850; i <= 9250; i += 100) {
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(i, -5750);
	}

	//上面房间
	for (int i = 4750; i <= 5150; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(i, -1650);
	}
	for (int i = 350; i <= 1350; i += 100)
	{
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(4450, -i);
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(4950, -i);
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(5450, -i);
	}
	for (int i = 4550; i <= 5350; i += 100)
	{
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(i, -350);
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(i, -850);
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(i, -1350);
	}

	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(4700, -500);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(4700, -1100);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(5200, -500);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(5200, -1050);

	//右下房间
	for (int i = 8850; i <= 9250; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(8250, -i);
	}
	for (int i = 8850; i <= 9250; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(i, -8250);
	}
	for (int i = 8550; i <= 9550; i += 100)
		for (int j = 8550; j <= 8750; j += 100)
		{
			newSprites.append(new ObstacleModel(8, 100));
			newSprites.back()->centerPos.setXY(i, -j);
		}
	for (int i = 8550; i <= 8750; i += 100)
		for (int j = 8750; j <= 9550; j += 100)
		{
			newSprites.append(new ObstacleModel(8, 100));
			newSprites.back()->centerPos.setXY(i, -j);
		}
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(9400, -9400);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(9550, -9100);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(8350, -9550);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(8850, -8400);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(9000, -9100);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(8350, -9100);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(8900, -9550);

	//中间房间
	for (int i = 4750; i <= 5150; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(6050, -i);
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(3850, -i);
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(i, -3850);
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(i, -6050);
	}
	for (int i = 4550; i <= 5350; i += 100)
		for (int j = 4550; j <= 5350; j += 100)
		{
			newSprites.append(new ObstacleModel(8, 100));
			newSprites.back()->centerPos.setXY(i, -j);
		}
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(5800, -4950);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(4250, -4100);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(4000, -4950);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(4300, -5850);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(4000, -5350);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(5800, -4100);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(5800, -5600);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(5000, -5850);

	//左房间
	for (int i = 4750; i <= 5150; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(1650, -i);
	}
	for (int i = 150; i <= 550; i += 100)
		for (int j = 3950; j <= 4650; j += 100)
		{
			newSprites.append(new ObstacleModel(8, 100));
			newSprites.back()->centerPos.setXY(i, -j);
			newSprites.append(new ObstacleModel(8, 100));
			newSprites.back()->centerPos.setXY(i + 1000, -j);
			newSprites.append(new ObstacleModel(8, 100));
			newSprites.back()->centerPos.setXY(i, -j - 1300);
			newSprites.append(new ObstacleModel(8, 100));
			newSprites.back()->centerPos.setXY(i + 1000, -j - 1300);
		}
	for (int i = 650; i <= 1050; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(i, -6050);
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(i, -i - 4100);
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(i, i - 5800);
	}
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(1200, -5050);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(1400, -4900);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(820, -4400);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(950, -5390);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(800, -5650);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(300, -5050);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(500, -4850);

	//中下房间
	for (int i = 4750; i <= 5150; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(i, -8250);
	}
	for (int i = 8850; i <= 9250; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(6050, -i);
	}
	for (int i = 8350; i <= 9450; i += 100)
	{
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(5450, -i);
		newSprites.append(new ObstacleModel(8, 100));
		newSprites.back()->centerPos.setXY(4450, -i - 300);
	}
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(4050, -9100);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(5680, -9100);
	newSprites.append(new EnemyPigModel);
	newSprites.back()->centerPos.setXY(5110, -9600);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(5800, -8800);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(5750, -9500);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(4100, -8600);
	newSprites.append(new EnemyMobModel);
	newSprites.back()->centerPos.setXY(5000, -9050);

	//左下房间
	for (int i = 650; i <= 1050; i += 100)
	{
		newSprites.append(new ObstacleModel(20, 100));
		newSprites.back()->centerPos.setXY(i, -8250);
	}

	// boss
	newSprites.append(new EnemyBossModel);
	newSprites.back()->centerPos.setXY(800, -9000);

	defaultFPS = 60;
	emit started();
}

PhysicsEngine::~PhysicsEngine()
{
	waitAndQuit();
	for each (SpriteModel *p in allSprites)
		delete p;
	physicsEngine = nullptr;
}

void PhysicsEngine::addBarrage(const MapPoint &start_pos, const MapVector &direction, BarrageType type)
{
	BulletModel *p;
	switch (type)
	{
	case ACTOR_BULLET:
		p = new BulletModel(false, 3, 8, 2000);
		break;
	case ENEMY_BULLET_SMALL:
		p = new BulletModel(true, 3, 8, 1600);
		break;
	case ENEMY_BULLET_BIG:
		p = new BulletModel(true, 5, 16, 1200);
		break;
	default:
		throw "没有这种类型";
	}
	p->centerPos = start_pos;
	p->setDirection(direction);
	addSprite(p);
}

void PhysicsEngine::addSprite(SpriteModel *p)
{
	newSprites.append(p);
}

MapPoint PhysicsEngine::getNearestActorPos(const MapPoint &enemyPos) const
{
	MapPoint result = actorsPos[0];
	double delta_s = (enemyPos - result).square();
	for (int i = 1; i < actorsPos.size(); i++)
	{
		if (distance_square(actorsPos[i], enemyPos) < distance_square(result, enemyPos))
			result = actorsPos[i];
	}
	return result;
}

void PhysicsEngine::mainloop()
{
	// 如果暂停则不做任何处理
	if (pauseGame)
		return;

	// 接收输入信息
	dataReceiver->receiveInputState(inputState);

	// 处理精灵动作
	for each (SpriteModel *p in allSprites)
		p->action();

	// 碰撞检测开始
	static const bool collision_kind_table[COLLISION_KIND_NUM][COLLISION_KIND_NUM] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 0, 1, 0, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 0, 1, 0, 1, 0 },
		{ 0, 0, 0, 0, 1, 1, 1, 1, 0, 0 },
		{ 1, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
		{ 1, 1, 0, 1, 0, 0, 1, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }
	}; // 碰撞关系矩阵
	// 0：地形 1：主角 2：敌人 3：箱子 4：主角子弹 5：敌方子弹 
	// 6：我方近战 7：敌方近战 8：地刺 9：出口关卡/关门触发/金币经验/拾取武器/开宝箱

	// 根据精灵的碰撞类型划分等价类
	QVector<SpriteModel*> collision_kinds[COLLISION_KIND_NUM];
	collision_kinds[0] = mapModel->getWalls();
	for each (SpriteModel *p in allSprites)
		collision_kinds[p->getCollisionKind()].append(p);

	// 处理各个等价类之间的碰撞检测
#ifdef COLLISION_COUNT
	collisionDecideCount = 0;
#endif

	ClosedRegion2D region(0, mapModel->getWidth(), -mapModel->getHeight(), 0);
	for (int i = 0; i < COLLISION_KIND_NUM; i++)
	{
		if (collision_kinds[i].size() > 1 && collision_kind_table[i][i])
		{
			collisionDecide(collision_kinds[i], region, true);
		}
		for (int j = i + 1; j < COLLISION_KIND_NUM; j++)
		{
			if (!(collision_kinds[i].empty() || collision_kinds[j].empty()) && (collision_kind_table[i][j] || collision_kind_table[j][i]))
			{
				collisionDecide(collision_kinds[i], collision_kinds[j], collision_kind_table[i][j], collision_kind_table[j][i], region, true);
			}
		}	
	}
#ifdef COLLISION_COUNT
	qDebug() << collisionDecideCount;
#endif

	// 更新精灵位置
	for each (SpriteModel *p in allSprites)
		p->updatePos(timePassed);

	// 边界检测
	for each (SpriteModel *p in allSprites)
		p->borderCheck(mapModel->getWidth(), mapModel->getHeight());

	// 更新时间
	for each (SpriteModel *p in allSprites)
		p->updateTime(timePassed);

	// 更新精灵信息
	dataSender->sendSpriteState(allSprites);
	dataSender->sendNewSpriteInfo(newSprites);

	// 销毁无用精灵
	for (auto it = allSprites.begin(); it != allSprites.end(); )
	{
		if ((*it)->toDelete)
		{
			delete *it;
			it = allSprites.erase(it);
		}
		else
		{
			++it;
		}
	}

	// 新精灵加入老精灵并清空新精灵列表
	allSprites += newSprites;
	newSprites.clear();
}

void PhysicsEngine::collisionDecide(QVector<SpriteModel*> &src)
{
	for (auto it1 = src.begin(); it1 != src.end(); ++it1)
	{
		for (auto it2 = it1 + 1; it2 != src.end(); ++it2)
		{
#ifdef COLLISION_COUNT
			collisionDecideCount++;
#endif
			if (SpriteModel::SketchyCollisionDetection(**it1, **it2))
			{
				double delta_t = timePassed;
				if ((*it1)->collisionDetection(**it2, delta_t))
				{
					(*it1)->dealCollision(**it2, delta_t);
					(*it2)->dealCollision(**it1, delta_t);
				}
			}
		}
	}
}

void PhysicsEngine::collisionDecide(QVector<SpriteModel*> &src1, QVector<SpriteModel*> &src2, bool i_j, bool j_i)
{
	for (auto it1 = src1.begin(); it1 != src1.end(); ++it1)
	{
		for (auto it2 = src2.begin(); it2 != src2.end(); ++it2)
		{
#ifdef COLLISION_COUNT
			collisionDecideCount++;
#endif
			if (SpriteModel::SketchyCollisionDetection(**it1, **it2))
			{
				double delta_t = timePassed;
				if ((*it1)->collisionDetection(**it2, delta_t))
				{
					if (i_j)
						(*it1)->dealCollision(**it2, delta_t);
					if (j_i)
						(*it2)->dealCollision(**it1, delta_t);
				}
			}
		}
	}
}

void PhysicsEngine::collisionDecide(QVector<SpriteModel*> &src, const ClosedRegion2D &region, bool vertical)
{
	if (src.size() <= 5 || region.area() < 100000)
	{
		if (src.size() > 1)
		{
			collisionDecide(src);
		}
	}
	else
	{
		if (vertical)
		{
			QVector<SpriteModel*> left, right, cross;
			double mid_x = region.x.mid() + (rand() - 16384) * 1e-5 * region.x.length();
			verticalDivide(src, mid_x, left, right, cross);
			collisionDecide(left, ClosedRegion2D(region.x.left, mid_x, region.y.left, region.y.right), false);
			collisionDecide(right, ClosedRegion2D(mid_x, region.x.right, region.y.left, region.y.right), false);
			collisionDecide(left, cross, true, true, region, false);
			collisionDecide(right, cross, true, true, region, false);
			collisionDecide(cross, region, false);
		}
		else
		{
			QVector<SpriteModel*> up, down, cross;
			double mid_y = region.y.mid() + (rand() - 16384) * 1e-5 * region.y.length();
			horizontalDivide(src, mid_y, up, down, cross);
			collisionDecide(down, ClosedRegion2D(region.x.left, region.x.right, region.y.left, mid_y), true);
			collisionDecide(up, ClosedRegion2D(region.x.left, region.x.right, mid_y, region.y.right), true);			
			collisionDecide(down, cross, true, true);
			collisionDecide(up, cross, true, true);
			collisionDecide(cross);
		}
	}
}

void PhysicsEngine::collisionDecide(QVector<SpriteModel*> &src1, QVector<SpriteModel*> &src2, bool i_j, bool j_i, const ClosedRegion2D &region, bool vertical)
{
	if (src1.size() * src2.size() <= 10 || region.area() < 100000)
	{
		if (!(src1.empty() || src2.empty()))
		{
			collisionDecide(src1, src2, i_j, j_i);
		}
	}
	else
	{
		if (vertical)
		{
			QVector<SpriteModel*> left1, right1, cross1, left2, right2, cross2;
			double mid_x = region.x.mid() + (rand() - 16384) * 1e-5 * region.x.length();
			verticalDivide(src1, mid_x, left1, right1, cross1);
			verticalDivide(src2, mid_x, left2, right2, cross2);
			collisionDecide(left1, left2, i_j, j_i, ClosedRegion2D(region.x.left, mid_x, region.y.left, region.y.right), false);
			collisionDecide(right1, right2, i_j, j_i, ClosedRegion2D(mid_x, region.x.right, region.y.left, region.y.right), false);
			collisionDecide(left1, cross2, i_j, j_i, region, false);
			collisionDecide(cross1, left2, i_j, j_i, region, false);
			collisionDecide(right1, cross2, i_j, j_i, region, false);
			collisionDecide(cross1, right2, i_j, j_i, region, false);
			collisionDecide(cross1, cross2, i_j, j_i, region, false);
		}
		else
		{
			QVector<SpriteModel*> up1, down1, cross1, up2, down2, cross2;
			double mid_y = region.y.mid() + (rand() - 16384) * 1e-5 * region.y.length();
			horizontalDivide(src1, mid_y, up1, down1, cross1);
			horizontalDivide(src2, mid_y, up2, down2, cross2);
			collisionDecide(down1, down2, i_j, j_i, ClosedRegion2D(region.x.left, region.x.right, region.y.left, mid_y), true);
			collisionDecide(up1, up2, i_j, j_i, ClosedRegion2D(region.x.left, region.x.right, mid_y, region.y.right), true);
			collisionDecide(down1, cross2, i_j, j_i);
			collisionDecide(cross1, down2, i_j, j_i);
			collisionDecide(up1, cross2, i_j, j_i);
			collisionDecide(cross1, up2, i_j, j_i);
			collisionDecide(cross1, cross2, i_j, j_i);
		}
	}
}

void PhysicsEngine::verticalDivide(const QVector<SpriteModel*> &src, double x, QVector<SpriteModel*> &left, QVector<SpriteModel*> &right, QVector<SpriteModel*> &cross)
{
	for (auto it = src.begin(); it != src.end(); ++it)
	{
		if ((*it)->maxX() < x)
			left.append(*it);
		else if ((*it)->minX() > x)
			right.append(*it);
		else
			cross.append(*it);
	}
}

void PhysicsEngine::horizontalDivide(const QVector<SpriteModel*> &src, double y, QVector<SpriteModel*> &up, QVector<SpriteModel*> &down, QVector<SpriteModel*> &cross)
{
	for (auto it = src.begin(); it != src.end(); ++it)
	{
		if ((*it)->maxY() < y)
			down.append(*it);
		else if ((*it)->minY() > y)
			up.append(*it);
		else
			cross.append(*it);
	}
}
