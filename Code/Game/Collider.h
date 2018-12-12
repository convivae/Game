#pragma once

class CircleShapedCollider;
class RectShapedCollider;

// 碰撞器的基类
class Collider
{
public:
	virtual ~Collider() {}

	virtual bool staticCollisionDetection(Collider &other, const MapPoint &p1, const MapPoint &p2) = 0;
	virtual bool staticCollisionDetection(CircleShapedCollider &other, const MapPoint &p1, const MapPoint &p2) { return false; }
	virtual bool staticCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2) { return false; }

	virtual bool dynamicCollisionDetection(Collider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) = 0;
	virtual bool dynamicCollisionDetection(CircleShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) { return false; }
	virtual bool dynamicCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) { return false; }

protected:
	Collider() {}
};

// 圆形碰撞器
class CircleShapedCollider : public Collider
{
public:
	CircleShapedCollider(double radius) : radius(radius) {}

	bool staticCollisionDetection(Collider &other, const MapPoint &p1, const MapPoint &p2) override { return other.staticCollisionDetection(*this, p1, p2); }
	bool staticCollisionDetection(CircleShapedCollider &other, const MapPoint &p1, const MapPoint &p2) override;
	bool staticCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2) override;

	bool dynamicCollisionDetection(Collider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) override { return other.dynamicCollisionDetection(*this, p1, p2, v1, v2, delta_t); }
	bool dynamicCollisionDetection(CircleShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) override;
	bool dynamicCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) override;

public:
	double radius;
};

// 矩形碰撞器，横平竖直的那种
class RectShapedCollider : public Collider
{
public:
	RectShapedCollider(double getWidth, double getHeight) 
		: halfWH(getWidth * 0.5, getHeight * 0.5) {}

	bool staticCollisionDetection(Collider &other, const MapPoint &p1, const MapPoint &p2) override { return other.staticCollisionDetection(*this, p1, p2); }
	bool staticCollisionDetection(CircleShapedCollider &other, const MapPoint &p1, const MapPoint &p2) override { return other.CircleShapedCollider::staticCollisionDetection(*this, p1, p2); }
	bool staticCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2) override;

	bool dynamicCollisionDetection(Collider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) override { return other.dynamicCollisionDetection(*this, p1, p2, v1, v2, delta_t); }
	bool dynamicCollisionDetection(CircleShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) { return other.CircleShapedCollider::dynamicCollisionDetection(*this, p1, p2, v1, v2, delta_t); }
	bool dynamicCollisionDetection(RectShapedCollider &other, const MapPoint &p1, const MapPoint &p2, const MapVector &v1, const MapVector &v2, double &delta_t) override;

public:
	MapVector halfWH;
};
