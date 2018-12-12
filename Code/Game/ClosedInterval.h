#pragma once

#include "LinearAlgebra.h"

// 求浮点数的平方
inline double square(double x)
{
	return x * x;
}

// 表示闭区间的结构体
struct ClosedInterval
{
	ClosedInterval(double x) : left(x), right(x) {}

	ClosedInterval(double left, double right) : left(left), right(right) 
	{
#ifdef _DEBUG
		if (left > right)
			throw "区间左边不能比右边大";
#endif
	}

	ClosedInterval &operator+=(const ClosedInterval &other)
	{
		this->left += other.left;
		this->right += other.right;
	}

	ClosedInterval &operator-=(const ClosedInterval &other)
	{
		this->left -= other.right;
		this->right -= other.left;
	}

	bool contain(const ClosedInterval &other) const
	{
		return this->left <= other.left && this->right >= other.right;
	}

	bool contain(double x) const
	{
		return this->left <= x && this->right >= x;
	}

	double mid() const
	{
		return (left + right) / 2;
	}

	double length() const
	{
		return right - left;
	}

	double left, right;
};

inline ClosedInterval operator+(const ClosedInterval &a, const ClosedInterval &b)
{
	return ClosedInterval(a.left + b.left, a.right + b.right);
}

inline ClosedInterval operator-(const ClosedInterval &a, const ClosedInterval &b)
{
	return ClosedInterval(a.left - b.right, a.right - b.left);
}

inline ClosedInterval operator-(const ClosedInterval &a)
{
	return ClosedInterval(-a.right, -a.left);
}

inline bool operator==(const ClosedInterval &a, const ClosedInterval &b)
{
	return a.left == b.left && a.right == b.right;
}

inline bool operator!=(const ClosedInterval &a, const ClosedInterval &b)
{
	return a.left != b.left || a.right != b.right;
}

inline bool intersect(const ClosedInterval &a, const ClosedInterval &b)
{
	return a.left <= b.right && a.right >= b.left;
}

// 表示二维闭区域的结构体
struct ClosedRegion2D
{
	ClosedRegion2D(const MapVector &v) : x(v.x, v.x), y(v.y, v.y) {}
	ClosedRegion2D(const MapPoint &p) : x(p.x, p.x), y(p.y, p.y) {}
	ClosedRegion2D(const ClosedInterval &x, const ClosedInterval &y) : x(x), y(y) {}
	ClosedRegion2D(double x, double y) : x(x, x), y(y, y) {}

	ClosedRegion2D(const MapVector &v_left, const MapVector &v_right)
		: x(v_left.x, v_right.x), y(v_left.y, v_right.y) {}
	ClosedRegion2D(const MapPoint &p_left, const MapPoint &p_right)
		: x(p_left.x, p_right.x), y(p_left.y, p_right.y) {}
	ClosedRegion2D(double x_left, double x_right, double y_left, double y_right)
		: x(x_left, x_right), y(y_left, y_right) {}

	bool contain(const ClosedRegion2D &other) const
	{
		return x.contain(other.x) && y.contain(other.y);
	}

	bool contain(const MapVector &v) const
	{
		return x.contain(v.x) && y.contain(v.y);
	}

	bool contain(const MapPoint &p) const
	{
		return x.contain(p.x) && y.contain(p.y);
	}

	double area() const
	{
		return x.length() * y.length();
	}

	ClosedInterval x, y;
};

inline ClosedRegion2D operator-(const ClosedRegion2D &a)
{
	return ClosedRegion2D(-a.x, -a.y);
}

inline bool operator==(const ClosedRegion2D &a, const ClosedRegion2D &b)
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const ClosedRegion2D &a, const ClosedRegion2D &b)
{
	return a.x != b.x || a.y != b.y;
}

inline bool intersect(const ClosedRegion2D &a, const ClosedRegion2D &b)
{
	return intersect(a.x, b.x) && intersect(a.y, b.y);
}
