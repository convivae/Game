#include "stdafx.h"
#include "Sprite.h"

bool HealthData::injured(int damage)
{
	if (health <= damage)
	{
		health = 0;
		return true;
	}
	else
	{
		health -= damage;
		return false;
	}
}

bool HealthDataWithShield::injured(int damage)
{
	if (shield < damage)
	{
		damage -= shield;
		shield = 0;
		return HealthData::injured(damage);		
	}
	else
	{
		shield -= damage;
		return false;
	}
}

bool MagicData::consume(int cost)
{
	if (cost > magic)
		return false;
	magic -= cost;
	return true;
}

void SpriteView::DrawBloodBar(QPainter &painter, QRect BloodRect, int health, int maxhp, BloodBarType type)
{
	if (maxhp < 0)
		return;

	double Current_HP = health;
	int H = BloodRect.height();

	switch (type)
	{
	case RED_BLOOD_BAR:
	case BLUE_MAGIC_BAR:
	case GRAY_SHELLED_BAR:
	{
		painter.setPen(QPen(Qt::black, 2 * H / 30));
		painter.setBrush(QBrush(QColor(139, 115, 85)));
		painter.drawRect(BloodRect);
		switch (type)
		{
		case RED_BLOOD_BAR:
			painter.setBrush(QBrush(Qt::red));
			break;
		case BLUE_MAGIC_BAR:
			painter.setBrush(QBrush(Qt::blue));
			break;
		case GRAY_SHELLED_BAR:
			painter.setBrush(QBrush(QColor(183, 183, 183)));
			break;
		}
		break;
	}
	case MOB_BLOOD_BAR:
	{
		painter.setPen(QPen(Qt::black, H / 8));
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(BloodRect);
		painter.setBrush(Qt::red);
		break;
	}
	case BOSS_BLOOD_BAR:
	{
		painter.setBrush(Qt::NoBrush);
		painter.setPen(QPen(Qt::black, 4 * H / 27));
		painter.drawRect(BloodRect);
		painter.setBrush(Qt::red);
		break;
	}
	}

	painter.drawRect(BloodRect.x(), BloodRect.y(), maxhp == 0 ? 0 : Current_HP * BloodRect.width() / maxhp, BloodRect.height());
	if (type != MOB_BLOOD_BAR)
	{
		QString str =
			QString::number(Current_HP) + QString("/") + QString::number(maxhp);

		QFont font;
		if (type != BOSS_BLOOD_BAR)
		{
			qint64 NumSize = H * 30 / 30;
			font.setFamily("Courier New");
			font.setPixelSize(NumSize);
			font.setLetterSpacing(QFont::AbsoluteSpacing, 5);
		}
		else
		{
			qint64 NumSize = H * 25 / 27;
			font.setFamily("Lucida Console");
			font.setPixelSize(NumSize);
			font.setLetterSpacing(QFont::AbsoluteSpacing, 3);
		}
		font.setBold(1);
		painter.setFont(font);
		painter.setPen(QPen(Qt::white, 1));
		painter.drawText(BloodRect, Qt::AlignCenter, str);
	}
}

void SpriteView::DrawBloodBackground(QPainter &painter, QRect rect)
{
	painter.setPen(QPen(Qt::black, rect.height() / 35));
	painter.setBrush(QBrush(QColor(238, 216, 174)));
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawRoundRect(rect);
}

bool SpriteModel::SketchyCollisionDetection(const SpriteModel &src1, const SpriteModel &src2)
{
	return intersect(src1.wrappedRect(), src2.wrappedRect());
}
