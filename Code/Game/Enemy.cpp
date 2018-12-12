#include "stdafx.h"
#include "global_vars.h"
#include "Enemy.h"
#include "Actor.h"
#include "Bullet.h"
#include "PhysicsEngine.h"

int EnemyView::bossHealth, EnemyView::bossMaxHealth;
QVector<QImage> EnemyView::monster_pictures[3];

EnemyView::EnemyView()
{
	last_vx = 0;
	centerPos = MapPoint::Zeros();
}

EnemyView::~EnemyView()
{
	bossHealth = 0;
}

void EnemyView::LoadResource()
{
	monster_pictures[0].resize(4);
	monster_pictures[1].resize(6);
	monster_pictures[2].resize(6);

	for (int i = 0; i < 4; i++)
	{
		QString source_file_name = QString("../images/monster/pig%1.png").arg(i);
		monster_pictures[0][i].load(source_file_name);
		if (monster_pictures[0][i].isNull())
			throw QString(u8"无法打开文件%1!").arg(source_file_name);
	}

	for (int i = 0; i < 6; i++)
	{
		QString source_file_name = QString("../images/monster/monster%1.png").arg(i);
		monster_pictures[1][i].load(source_file_name);
		if (monster_pictures[1][i].isNull())
			throw QString(u8"无法打开文件%1!").arg(source_file_name);
	}

	for (int i = 0; i < 6; i++)
	{
		QString source_file_name = QString("../images/monster/boss%1.png").arg(i);
		monster_pictures[2][i].load(source_file_name);
		if (monster_pictures[2][i].isNull())
			throw QString(u8"无法打开文件%1!").arg(source_file_name);
	}
}

void EnemyView::readStream(QDataStream &spriteStateData)
{
	monsterLastPos = centerPos;
	spriteStateData >> centerPos.x >> centerPos.y >> health;
	bool is_new;
	spriteStateData >> is_new;
	if (is_new) 
	{
		spriteStateData >> pictureIndex >> maxHealth >> isBoss;
		monsterLastPos = centerPos;
	}
	if (isBoss)
	{
		bossHealth = health;
		bossMaxHealth = maxHealth;
	}
}

void EnemyView::drawBody(QPainter &painter, const M2S_Transformation &f)
{
	MapVector v = centerPos - monsterLastPos;
	ScreenPoint p = f(centerPos);

	ScreenVector radius;
	if (pictureIndex == 2)
		radius = f(MapVector(250, -250));
	else
		radius = f(MapVector(150, -150));

	QRectF target;
	if (pictureIndex == 0)
		target = QRect(p.x - radius.x / 2.3, p.y - radius.y / 2.3, radius.x, radius.y);
	else if (pictureIndex == 1)
		target = QRect(p.x - radius.x / 1.8, p.y - radius.y / 2.5, radius.x, radius.y);
	else if (pictureIndex == 2)
		target = QRect(p.x - radius.x / 2.0, p.y - radius.y / 2.0, radius.x, radius.y);

	QRectF source(0.0, 0.0, 100.0, 100.0);
	int half_size = monster_pictures[pictureIndex].size() / 2;

	if (v.x != 0)
	{
		painter.drawImage(target, monster_pictures[pictureIndex][clock() / 100 % half_size + (v.x > 0 ? half_size : 0)], source);
		last_vx = v.x;
	}
	else
	{
		if (last_vx < 0)
		{
			if (v.y == 0)
				painter.drawImage(target, monster_pictures[pictureIndex][0], source);
			else
				painter.drawImage(target, monster_pictures[pictureIndex][clock() / 100 % half_size + half_size], source);
		}
		else
		{
			if (v.y == 0)
				painter.drawImage(target, monster_pictures[pictureIndex][3], source);
			else
				painter.drawImage(target, monster_pictures[pictureIndex][clock() / 100 % half_size], source);
		}
	}
}

void EnemyView::drawBloodBar(QPainter &painter, const M2S_Transformation &f)
{
	MapPoint blood_bar_top_left = centerPos + MapVector(-70, 90);
	MapPoint blood_bar_bottom_right = centerPos + MapVector(55, 80);
	QRect rect(f(blood_bar_top_left).toQPoint(), f(blood_bar_bottom_right).toQPoint());
	DrawBloodBar(painter, rect, health, maxHealth, MOB_BLOOD_BAR);
}

void EnemyModel::writeStream(QDataStream &spriteStateData) const
{
	spriteStateData << centerPos.x << centerPos.y << healthData.health;
	spriteStateData << isNew;
	if (isNew)
		spriteStateData << pictureIndex << healthData.maxHealth << isBoss;
}

bool EnemyModel::collisionDetection(SpriteModel &other, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, velocity, delta_t);
}

bool EnemyModel::staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, p2, velocity, MapVector::Zeros(), delta_t);
}

bool EnemyModel::dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, p2, velocity, v2, delta_t);
}

void EnemyModel::dealCollision(SpriteModel &other, double t)
{
	switch (other.getType())
	{
	case SPRITE_ACTOR:
	{
		// 与角色靠近会停下来
		if (t < collisionTime)
		{
			collisionTime = t;
			collised = true;
		}
		break;
	}
	case SPRITE_BULLET:
	{
		BulletModel &bullet = *static_cast<BulletModel*>(&other);
		toDelete = healthData.injured(bullet.damage);
		break;
	}
	case SPRITE_OBSTACLE:
	case SPRITE_WALL:
	{
		// 碰到障碍或墙不允许穿模，但碰在一起可以移动并分开
		if (cos_angle(velocity, centerPos - other.centerPos) < 0.7)
		{
			if (t < collisionTime)
			{
				collisionTime = t;
				collised = true;
			}
		}
		break;
	}
	case SPRITE_ENEMY:
	{
		// 敌人或敌人互相靠近时相碰不允许穿模，但碰在一起可以移动并分开
		EnemyModel &another_enemy = *static_cast<EnemyModel*>(&other);
		if (dot(velocity - another_enemy.velocity, centerPos - another_enemy.centerPos) < 0)
		{
			if (t < collisionTime)
			{
				collisionTime = t;
				collised = true;
			}
		}
	}
	}
}

void EnemyModel::updatePos(double delta_t)
{
	centerPos += velocity * (collised ? collisionTime : delta_t);
	collisionTime = INFINITY;
	collised = false;
}

void EnemyModel::borderCheck(double map_width, double map_height)
{
	if (centerPos.x > map_width)
		centerPos.x = map_width;
	else if (centerPos.x < 0)
		centerPos.x = 0;
	if (centerPos.y < -map_height)
		centerPos.y = -map_height;
	else if (centerPos.y > 0)
		centerPos.y = 0;
}

EnemyPigModel::EnemyPigModel()
	: EnemyModel(0, false, 12, 1, 50, 300)
{
}

void EnemyPigModel::action()
{
	MapVector delta_r = physicsEngine->getNearestActorPos(centerPos) - centerPos;
	double delta_s = delta_r.length();

	if (delta_s <= 1414)
		velocity = delta_r.normalize() * speed; // 靠近主角
	else
		velocity = MapVector::Zeros();
}

EnemyMobModel::EnemyMobModel()
	: EnemyModel(1, false, 16, 1, 50, 200)
{
	cooldownWeapon.totalTime[0] = 2;
}

void EnemyMobModel::action()
{
	MapVector delta_r = physicsEngine->getNearestActorPos(centerPos) - centerPos;
	double delta_s = delta_r.length();

	if (1000 <= delta_s && delta_s <= 1732)
		velocity = delta_r.normalize() * speed; // 靠近主角
	else if (delta_s <= 548)
		velocity = delta_r.normalize() * -speed; // 远离主角
	else
		velocity = MapVector::Zeros();

	// 向主角发子弹
	if (delta_s <= 1414 && cooldownWeapon.remainTime <= 0)
	{
		cooldownWeapon.remainTime = cooldownWeapon.totalTime[0];
		physicsEngine->addBarrage(centerPos + delta_r.normalize() * collider.radius, delta_r, ENEMY_BULLET_SMALL);
	}
}

void EnemyMobModel::updateTime(double delta_t)
{
	cooldownWeapon.remainTime -= delta_t;
}

EnemyBossModel::EnemyBossModel()
	: EnemyModel(2, true, 500, 2, 80, 100)
{
	cooldownWeapon.totalTime[0] = 1.5;
	cooldownSkill[0].totalTime[0] = 4.2;
	cooldownSkill[1].totalTime[0] = 12.3;
}

void EnemyBossModel::action()
{
	MapVector delta_r = physicsEngine->getNearestActorPos(centerPos) - centerPos;
	double delta_s = delta_r.length();

	if (1000 <= delta_s && delta_s <= 1732)
		velocity = delta_r.normalize() * speed; // 靠近主角
	else if (delta_s <= 707)
		velocity = delta_r.normalize() * -speed; // 远离主角
	else
		velocity = MapVector::Zeros();

	// 向主角发子弹
	if (delta_s <= 1414 && cooldownWeapon.remainTime <= 0)
	{
		cooldownWeapon.remainTime = cooldownWeapon.totalTime[0];
		physicsEngine->addBarrage(centerPos + delta_r.normalize() * collider.radius, delta_r, ENEMY_BULLET_BIG);
		MapVector r1 = LinearTransformation::Rotate(M_PI / 8)(delta_r);
		physicsEngine->addBarrage(centerPos + r1.normalize() * collider.radius, r1, ENEMY_BULLET_BIG);
		MapVector r2 = LinearTransformation::Rotate(-M_PI / 8)(delta_r);
		physicsEngine->addBarrage(centerPos + r2.normalize() * collider.radius, r2, ENEMY_BULLET_BIG);
	}

	// 弹幕技能
	if (delta_s <= 1732 && cooldownSkill[0].remainTime <= 0)
	{
		cooldownSkill[0].remainTime = cooldownSkill[0].totalTime[0];
		physicsEngine->addBarrage(centerPos + delta_r.normalize() * collider.radius, delta_r, ENEMY_BULLET_SMALL);
		const int n = 24;
		MapVector r1 = delta_r;
		LinearTransformation f = LinearTransformation::Rotate(2 * M_PI / n);
		for (int i = 1; i < n; i++)
		{
			r1 = f(r1);
			physicsEngine->addBarrage(centerPos + r1.normalize() * collider.radius, r1, ENEMY_BULLET_SMALL);
		}
	}

	// 召唤技能
	if (healthData.health <= healthData.maxHealth / 2 && delta_s <= 1414 && cooldownSkill[1].remainTime <= 0)
	{
		cooldownSkill[1].remainTime = cooldownSkill[1].totalTime[0];
		EnemyMobModel *p = new EnemyMobModel;
		p->centerPos = centerPos + MapVector(300, 300);
		physicsEngine->addSprite(p);
		p = new EnemyMobModel;
		p->centerPos = centerPos + MapVector(-300, 300);
		physicsEngine->addSprite(p);
		p = new EnemyMobModel;
		p->centerPos = centerPos + MapVector(300, -300);
		physicsEngine->addSprite(p);
		p = new EnemyMobModel;
		p->centerPos = centerPos + MapVector(-300, -300);
		physicsEngine->addSprite(p);
	}
}

void EnemyBossModel::updateTime(double delta_t)
{
	cooldownWeapon.remainTime -= delta_t;
	cooldownSkill[0].remainTime -= delta_t;
	cooldownSkill[1].remainTime -= delta_t;
}
