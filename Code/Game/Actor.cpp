#include "stdafx.h"
#include "global_vars.h"
#include "Actor.h"
#include "Bullet.h"
#include "Obstacle.h"
#include "Enemy.h"
#include "PhysicsEngine.h"

#define NUM_OF_ACTOR_PICTURES 8
#define NUM_OF_ONE_SIDE_PER_ACTOR 4

ActorView *ActorView::myActor;
QVector<QImage> ActorView::actor_pictures[2];

ActorView::ActorView()
{
	last_vx = 0;
	centerPos = MapPoint::Zeros();
}

ActorView::~ActorView()
{
	if (id == myID)
		myActor = nullptr;
}

void ActorView::LoadResource()
{
	actor_pictures[0].resize(NUM_OF_ACTOR_PICTURES);
	actor_pictures[1].resize(NUM_OF_ACTOR_PICTURES);

	for (int i = 0; i < NUM_OF_ONE_SIDE_PER_ACTOR; i++) 
	{
		QString source_file_name = QString("../images/heros/one_l%1.png").arg(i);
		actor_pictures[0][i].load(source_file_name);
		if (actor_pictures[0][i].isNull())
			throw QString(u8"无法打开文件%1!").arg(source_file_name);

		source_file_name = QString("../images/heros/two_l%1.png").arg(i);
		actor_pictures[1][i].load(source_file_name);
		if (actor_pictures[1][i].isNull())
			throw QString(u8"无法打开文件%1!").arg(source_file_name);
	}

	for (int i = NUM_OF_ONE_SIDE_PER_ACTOR; i < NUM_OF_ACTOR_PICTURES; i++) 
	{
		QString source_file_name = QString("../images/heros/one_r%1.png").arg(i - 4);
		actor_pictures[0][i].load(source_file_name);
		if (actor_pictures[0][i].isNull())
			throw QString(u8"无法打开文件%1!").arg(source_file_name);

		source_file_name = QString("../images/heros/two_r%1.png").arg(i - 4);
		actor_pictures[1][i].load(source_file_name);
		if (actor_pictures[1][i].isNull())
			throw QString(u8"无法打开文件%1!").arg(source_file_name);
	}
}

void ActorView::readStream(QDataStream &spriteStateData)
{
	actorLastPos = centerPos;
	spriteStateData >> centerPos.x >> centerPos.y >> health >> shield >> magic;
	bool is_new;
	spriteStateData >> is_new;
	if (is_new) 
	{
		spriteStateData >> id >> pictureIndex >> maxHealth >> maxShield >> maxMagic;
		actorLastPos = centerPos;
		if (id == myID)
			myActor = this;
	}
}

void ActorView::drawBody(QPainter &painter, const M2S_Transformation &f)
{
	MapVector v = centerPos - actorLastPos;
	ScreenPoint p = f(centerPos);
	ScreenVector radius = f(MapVector(150, -150));

	if (v.x != 0)
		last_vx = v.x;

	QRectF target(p.x - radius.x / 2, p.y - radius.y / 2, radius.x, radius.y);
	QRectF source(0.0, 0.0, 100.0, 100.0);

	if (v.x == 0) 
	{
		if (last_vx < 0) {
			if (v.y == 0)
				painter.drawImage(target, actor_pictures[pictureIndex][0], source);
			else {
				painter.drawImage(target, actor_pictures[pictureIndex][clock() / 100 % 4], source);
			}
		}
			
		else {
			if(v.y == 0)
				painter.drawImage(target, actor_pictures[pictureIndex][4], source);
			else
				painter.drawImage(target, actor_pictures[pictureIndex][clock() / 100 % 4 + 4], source);
		}
	}
	else if (v.x > 0) 
	{
		painter.drawImage(target, actor_pictures[pictureIndex][clock() / 100 % 4 + 4], source);
	}
	else 
	{
		painter.drawImage(target, actor_pictures[pictureIndex][clock() / 100 % 4], source);
	}
}

void ActorView::drawBloodBar(QPainter &painter, const M2S_Transformation &f)
{
	MapPoint blood_bar_top_left = centerPos + MapVector(-70, 100);
	MapPoint blood_bar_bottom_right = centerPos + MapVector(70, 90);
	QRect rect(f(blood_bar_top_left).toQPoint(), f(blood_bar_bottom_right).toQPoint());
	DrawBloodBar(painter, rect, health + shield, maxHealth + maxShield, MOB_BLOOD_BAR);
}

void ActorView::drawBloodBar(QPainter &painter, int x, int y, int w, int h)
{
	DrawBloodBackground(painter, QRect(x, y, w, h));
	DrawBloodBar(painter, QRect(x + w / 10, y + h / 7, w * 8 / 10, h / 7), health, maxHealth, RED_BLOOD_BAR);
	DrawBloodBar(painter, QRect(x + w / 10, y + h * 3 / 7, w * 8 / 10, h / 7), shield, maxShield, GRAY_SHELLED_BAR);
	DrawBloodBar(painter, QRect(x + w / 10, y + h * 5 / 7, w * 8 / 10, h / 7), magic, maxMagic, BLUE_MAGIC_BAR);
}

void ActorModel::writeStream(QDataStream &spriteStateData) const
{
	spriteStateData << centerPos.x << centerPos.y << healthData.health << healthData.shield << magicData.magic;
	spriteStateData << isNew;
	if (isNew)
		spriteStateData << id << pictureIndex << healthData.maxHealth << healthData.maxShield << magicData.maxMagic;
}

void ActorModel::action()
{
	if (healthData.health == 0)
	{
		toDelete = true;
		posInfo->x = INFINITY;
		posInfo->y = INFINITY;
		return;
	}

	MapVector direction((double)ctrlInfo->key.x, (double)ctrlInfo->key.y);
	if (ctrlInfo->key.x != 0 && ctrlInfo->key.y != 0)
		direction = direction.normalize();
	velocity = direction * speed;

	// 鼠标点击有效位置并且武器冷却时间已完成，则处理角色攻击
	if (ctrlInfo->mouse.pressed && cooldownWeapon.remainTime <= 0)
	{
		// 需要耗蓝，否则无法攻击
		if (magicData.consume(1))
		{
			cooldownWeapon.remainTime = cooldownWeapon.totalTime[0];
			MapVector direction = MapPoint(ctrlInfo->mouse.x, ctrlInfo->mouse.y) - centerPos;
			physicsEngine->addBarrage(centerPos + direction.normalize() * collider.radius, direction, ACTOR_BULLET);
		}
	}
}

bool ActorModel::collisionDetection(SpriteModel &other, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, velocity, delta_t);
}

bool ActorModel::staticCollisionDetection(Collider &other, const MapPoint &p2, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, p2, velocity, MapVector::Zeros(), delta_t);
}

bool ActorModel::dynamicCollisionDetection(Collider &other, const MapPoint &p2, const MapVector &v2, double &delta_t)
{
	return other.dynamicCollisionDetection(collider, centerPos, p2, velocity, v2, delta_t);
}

void ActorModel::dealCollision(SpriteModel &other, double t)
{
	switch (other.getType())
	{
	case SPRITE_ACTOR:
	{
		// 角色互相靠近时相碰不允许穿模，但碰在一起可以移动并分开
		ActorModel &another_actor = *static_cast<ActorModel*>(&other);
		if (dot(velocity - another_actor.velocity, centerPos - another_actor.centerPos) < 0)
		{
			if (t < collisionTime)
			{
				collisionTime = t;
				collised = true;
			}
		}
		break;
	}
	case SPRITE_BULLET:
	{
		// 角色中弹掉血，但是短时间内多次中弹不会连续掉血
		if (cooldownDamage.remainTime <= 0)
		{
			cooldownDamage.remainTime = cooldownDamage.totalTime[0];
			cooldownShield.remainTime = cooldownShield.totalTime[1];
			BulletModel &bullet = *static_cast<BulletModel*>(&other);
			healthData.injured(bullet.damage);
		}
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
		// 角色与敌人靠近时会有碰撞伤害
		EnemyModel &enemy = *static_cast<EnemyModel*>(&other);
		if (cooldownDamage.remainTime <= 0)
		{
			cooldownDamage.remainTime = cooldownDamage.totalTime[0];
			cooldownShield.remainTime = cooldownShield.totalTime[1];
			healthData.injured(enemy.damage);
		}
		break;
	}
	}	
}

void ActorModel::updatePos(double delta_t)
{
	if (!toDelete)
	{
		centerPos += velocity * (collised ? collisionTime : delta_t);
		*posInfo = centerPos;
		collisionTime = INFINITY;
		collised = false;
	}
}

void ActorModel::borderCheck(double map_width, double map_height)
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

void ActorModel::updateTime(double delta_t)
{
	// 更新冷却时间
	cooldownWeapon.remainTime -= delta_t;
	cooldownDamage.remainTime -= delta_t;
	cooldownShield.remainTime -= delta_t;

	// 回复护盾
	if (cooldownShield.remainTime <= 0)
	{
		cooldownShield.remainTime = cooldownShield.totalTime[0];
		healthData.shield = healthData.shield >= healthData.maxShield ? healthData.maxShield : healthData.shield + 1;
	}
}

ActorMagicianModel::ActorMagicianModel(InputState *ctrlInfo, MapPoint *posInfo, int id)
	: ActorModel(ctrlInfo, posInfo, id, 0, 30, 20, 500, 60, 800)
{
	cooldownWeapon.totalTime[0] = 0.2;
	cooldownDamage.totalTime[0] = 0.1;
	cooldownShield.totalTime[0] = 1.0;
	cooldownShield.totalTime[1] = 5.0;
}
