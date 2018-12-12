#include "stdafx.h"
#include "Collider.h"

bool CircleShapedCollider::staticCollisionDetection(CircleShapedCollider &other, const MapPoint &p1, const MapPoint &p2)
{
	return distance_square(p1, p2) < square(this->radius + other.radius);
}

bool CircleShapedCollider::staticCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2)
{
	// 以圆形为基准，求出矩形的相对位置
	MapVector v0 = p2 - p1;

	// 找出矩形上距离圆心最近的点的位移
	MapVector v = vmin(vmax(MapVector::Zeros(), v0 - other.halfWH), v0 + other.halfWH);
	
	// 判断这点与圆心的距离是否小于半径
	return v.square() < square(radius);
}

bool CircleShapedCollider::dynamicCollisionDetection(CircleShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t)
{
	// 处理相对静止的特殊情况
	if (v1 == v2)
	{
		bool collision = CircleShapedCollider::staticCollisionDetection(other, p1, p2);
		delta_t = collision ? 0 : delta_t;
		return collision;
	}		 

	// 计算相对位移和速度
	MapVector r = p1 - p2, v = v1 - v2;

	// 计算关于t的二次函数（圆心距离平方减去半径和的平方）的系数
	double a = v.square(), half_b = dot(r, v), c = r.square() - square(this->radius + other.radius);

	// 判断该二次函数是否有零点，无零点则一定不会碰撞，有两个相同零点则为擦边，认为不碰撞
	double delta_4 = square(half_b) - a * c;
	if (delta_4 <= 0)
		return false;

	// 求出临界时间
	double sqrt_delta_4 = sqrt(delta_4);
	double t_begin = (-half_b - sqrt_delta_4) / a, t_end = (-half_b + sqrt_delta_4) / a;

	// 如果碰撞时间当前帧结束前，且分离时间当前帧开始后，则会发生碰撞，并记录具体碰撞时间
	if (t_begin <= delta_t && t_end > 0)
	{
		delta_t = t_begin > 0 ? t_begin * 0.99 : 0;
		return true;
	}

	// 其他情况则判定为不碰撞
	return false;
}

bool CircleShapedCollider::dynamicCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t)
{
	// 处理相对静止的特殊情况
	if (v1 == v2)
	{
		bool collision = CircleShapedCollider::staticCollisionDetection(other, p1, p2);
		delta_t = collision ? 0 : delta_t;
		return collision;
	}

	// 采取近似处理
	double t1 = delta_t, t2 = delta_t;
	RectShapedCollider tmp1(radius * 2, radius * 2);
	if (tmp1.RectShapedCollider::dynamicCollisionDetection(other, p1, p2, v1, v2, t1))
	{
		CircleShapedCollider tmp2(other.halfWH.length());
		if (tmp2.CircleShapedCollider::dynamicCollisionDetection(*this, p1, p2, v1, v2, t2))
		{
			delta_t = t1 > t2 ? t1 : t2;
			return true;
		}
	}
	return false;
}

bool RectShapedCollider::staticCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2)
{
	ClosedRegion2D region1(p1 - this->halfWH, p1 + this->halfWH), region2(p2 - other.halfWH, p2 + other.halfWH);
	return intersect(region1, region2);
}

bool RectShapedCollider::dynamicCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t)
{
	// 处理相对静止的特殊情况
	if (v1 == v2)
	{
		bool collision = RectShapedCollider::staticCollisionDetection(other, p1, p2);
		delta_t = collision ? 0 : delta_t;
		return collision;
	}

	// 分别计算两矩形的左下角和右上角的坐标
	MapPoint left1 = p1 - this->halfWH, right1 = p1 + this->halfWH, left2 = p2 - other.halfWH, right2 = p2 + other.halfWH;

	// 计算相对速度
	MapVector v = v1 - v2;

	// 分别计算x和y方向上相交的时间
	MapVector t1xy = (left2 - right1).times(v), t2xy = (right2 - left1).times(v);
	MapVector tminxy = vmin(t1xy, t2xy), tmaxxy = vmax(t1xy, t2xy);
	ClosedInterval ix(tminxy.x, tmaxxy.x), iy(tminxy.y, tmaxxy.y);

	// 如果两区间无交集，则不会碰撞，若v.x或v.y为0，则某一维数据无效
	if (v.x == 0)
	{
		if (left1.x >= right2.x || left2.x >= right1.x)
			return false;
		else
			ix = ClosedInterval(-INFINITY, INFINITY);
	}
	else if (v.y == 0)
	{
		if (left1.y >= right2.y || left2.y >= right1.y)
			return false;
		else
			iy = ClosedInterval(-INFINITY, INFINITY);
	}
	else
	{
		if (!intersect(ix, iy))
			return false;
	}

	// 否则，两集合的交集则为碰撞的时间
	double t_begin = ix.left > iy.left ? ix.left : iy.left;
	double t_end = ix.right < iy.right ? ix.right : iy.right;

	// 如果碰撞时间当前帧结束前，且分离时间当前帧开始后，则会发生碰撞，并记录具体碰撞时间
	if (t_begin <= delta_t && t_end > 0)
	{
		delta_t = t_begin > 0 ? t_begin * 0.99 : 0;
		return true;
	}

	// 其他情况则判定为不碰撞
	return false;
}
