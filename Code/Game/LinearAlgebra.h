#pragma once

#include <immintrin.h>

// 项目中向量分为地图坐标系和屏幕坐标系，防止混用导致错误而设
enum GameVectorType
{
	MAP_VECTOR, SCREEN_VECTOR
};

// 定义向量类和点类的存储结构和通用接口
class alignas(16) GameVectorBase
{
protected:
	GameVectorBase() {}

public:
	void setXY(double x, double y)
	{
		packed() = _mm_unpacklo_pd(_mm_load_sd(&x), _mm_load_sd(&y));
	}

	QString toString() const
	{
		return QString("(%1, %2)").arg(x).arg(y);
	}

	bool isZero() const
	{
		return x == 0.0 && y == 0.0;
	}

	// 返回该点或向量在哪个象限，如果在原点或坐标轴上则返回0
	int quadrant() const
	{
		if (x > 0.0)
		{
			if (y > 0.0)
				return 1;
			else if (y < 0.0)
				return 2;
		}
		else if (x < 0.0)
		{
			if (y > 0.0)
				return 4;
			else if (y < 0.0)
				return 3;
		}
		return 0;
	}

	__m128d &packed() { return *(__m128d*)&x; }
	const __m128d &packed() const { return *(__m128d*)&x; }

public:
	double x, y;
};

// 向量类，用于表示位移、速度、加速度等，不用来表示点坐标
// MapVector是使用地图坐标系，ScreenVector是使用屏幕坐标系，表示点坐标请用MapPoint和ScreenPoint
template<GameVectorType T>
class alignas(16) GameVector : public GameVectorBase
{
public:
	GameVector() {} // 不会初始化为零向量

	GameVector(__m128d src)
	{
		packed() = src;
	}

	GameVector(double x, double y)
	{
		setXY(x, y);
	}

	GameVector(const GameVector &src)
	{
		packed() = src.packed();
	}

	// 返回零向量
	static GameVector Zeros()
	{
		return { 0.0, 0.0 };
	}

	GameVector &operator=(const GameVector &src)
	{
		packed() = src.packed();
		return *this;
	}

	GameVector &operator+=(const GameVector &other)
	{
		packed() = _mm_add_pd(packed(), other.packed());
		return *this;
	}

	GameVector &operator-=(const GameVector &other)
	{
		packed() = _mm_sub_pd(packed(), other.packed());
		return *this;
	}

	GameVector &operator*=(double k)
	{
		packed() = _mm_mul_pd(packed(), _mm_broadcastsd_pd(_mm_load_sd(&k)));
		return *this;
	}

	GameVector &operator/=(double k)
	{
		packed() = _mm_div_pd(packed(), _mm_broadcastsd_pd(_mm_load_sd(&k)));
		return *this;
	}

	// 返回伸缩变换后的向量，即对应项相乘，不改变自身
	GameVector stretch(const GameVector &other) const
	{
		return _mm_mul_pd(packed(), other.packed());
	}

	// 返回该向量的x和y是另一向量x和y的多少倍，即对应项相除，不改变自身
	GameVector times(const GameVector &other) const
	{
		return _mm_div_pd(packed(), other.packed());
	}

	// 模长的平方
	double square() const
	{
		double result;
		__m128d tmp = _mm_mul_pd(packed(), packed());
		_mm_store_sd(&result, _mm_hadd_pd(tmp, tmp));
		return result;
	}

	// 求模长
	double length() const
	{
		double result;
		__m128d tmp = _mm_mul_pd(packed(), packed());
		_mm_store_sd(&result, _mm_sqrt_pd(_mm_hadd_pd(tmp, tmp)));
		return result;
	}

	// 返回与此向量同方向的单位向量，不改变自身
	GameVector normalize() const
	{
		__m128d tmp = _mm_mul_pd(packed(), packed());
		return _mm_div_pd(packed(), _mm_sqrt_pd(_mm_hadd_pd(tmp, tmp)));
	}

	// 返回此向量在另一向量方向上的投影，不改变自身
	double projection(const GameVector &other) const
	{
		double result;
		__m128d t1 = _mm_mul_pd(this->packed(), other.packed());
		__m128d t2 = _mm_mul_pd(other.packed(), other.packed());
		_mm_store_sd(&result, _mm_div_pd(_mm_hadd_pd(t1, t1), _mm_sqrt_pd(_mm_hadd_pd(t2, t2))));
		return result;
	}
};

template<GameVectorType T>
inline GameVector<T> operator+(const GameVector<T> &a, const GameVector<T> &b)
{
	return _mm_add_pd(a.packed(), b.packed());
}

template<GameVectorType T>
inline GameVector<T> operator-(const GameVector<T> &a, const GameVector<T> &b)
{
	return _mm_sub_pd(a.packed(), b.packed());
}

template<GameVectorType T>
inline GameVector<T> operator-(const GameVector<T> &a)
{
	return _mm_sub_pd({ 0.0, 0.0 }, a.packed());
}

template<GameVectorType T>
inline GameVector<T> operator*(const GameVector<T> &a, double k)
{
	return _mm_mul_pd(a.packed(), _mm_broadcastsd_pd(_mm_load_sd(&k)));
}

template<GameVectorType T>
inline GameVector<T> operator*(double k, const GameVector<T> &a)
{
	return a * k;
}

template<GameVectorType T>
inline GameVector<T> operator/(const GameVector<T> &a, double k)
{
	return _mm_div_pd(a.packed(), _mm_broadcastsd_pd(_mm_load_sd(&k)));
}

// 分别对x和y求绝对值
template<GameVectorType T>
inline GameVector<T> vabs(const GameVector<T> &a)
{
	const quint64 mask[2] = { 0x7fffffffffffffff, 0x7fffffffffffffff };
	return _mm_and_pd(*(__m128d*)mask, a.packed());
}

// 分别对x和y求绝对值的相反数
template<GameVectorType T>
inline GameVector<T> vnabs(const GameVector<T> &a)
{
	const quint64 mask[2] = { 0x8000000000000000, 0x8000000000000000 };
	return _mm_or_pd(*(__m128d*)mask, a.packed());
}

// 分别对每一维取大的值
template<GameVectorType T>
inline GameVector<T> vmax(const GameVector<T> &a, const GameVector<T> &b)
{
	return _mm_max_pd(a.packed(), b.packed());
}

// 分别对每一维取小的值
template<GameVectorType T>
inline GameVector<T> vmin(const GameVector<T> &a, const GameVector<T> &b)
{
	return _mm_min_pd(a.packed(), b.packed());
}

template<GameVectorType T>
inline bool operator==(const GameVector<T> &a, const GameVector<T> &b)
{
	return a.x == b.x && a.y == b.y;
}

template<GameVectorType T>
inline bool operator!=(const GameVector<T> &a, const GameVector<T> &b)
{
	return a.x != b.x || a.y != b.y;
}

// 求两向量内积
template<GameVectorType T>
double dot(const GameVector<T> &a, const GameVector<T> &b)
{
	double result;
	__m128d tmp = _mm_mul_pd(a.packed(), b.packed());
	_mm_store_sd(&result, _mm_hadd_pd(tmp, tmp));
	return result;
}

// 求两向量夹角余弦
template<GameVectorType T>
double cos_angle(const GameVector<T> &a, const GameVector<T> &b)
{
	return dot(a, b) / sqrt(a.square() * b.square());
}

// 求两向量外积
template<GameVectorType T>
double cross(const GameVector<T> &a, const GameVector<T> &b)
{
	return a.x * b.y - a.y * b.x;
}

// 求两向量夹角正弦
template<GameVectorType T>
double sin_angle(const GameVector<T> &a, const GameVector<T> &b)
{
	return cross(a, b) / sqrt(a.square() * b.square());
}

// 点坐标类，专门用来表示点坐标，与向量类接口有区别
template<GameVectorType T>
class alignas(16) GamePoint : public GameVectorBase
{
public:
	GamePoint() {} // 不会初始化为原点

	GamePoint(__m128d src)
	{
		packed() = src;
	}

	GamePoint(double x, double y)
	{
		packed() = _mm_unpacklo_pd(_mm_load_sd(&x), _mm_load_sd(&y));
	}

	GamePoint(const GamePoint &src)
	{
		packed() = src.packed();
	}

	// 返回原点
	static GamePoint Zeros()
	{
		return { 0.0, 0.0 };
	}

	QPointF toQPointF() const
	{
		return QPointF(x, y);
	}

	QPoint toQPoint() const
	{
		return QPoint(x, y);
	}

	GamePoint &operator=(const GamePoint &src)
	{
		packed() = src.packed();
		return *this;
	}

	GamePoint &operator+=(const GameVector<T> &offset)
	{
		packed() = _mm_add_pd(packed(), offset.packed());
		return *this;
	}

	GamePoint &operator-=(const GameVector<T> &offset)
	{
		packed() = _mm_sub_pd(packed(), offset.packed());
		return *this;
	}
};

template<GameVectorType T>
inline GamePoint<T> operator+(const GamePoint<T> &point, const GameVector<T> &offset)
{
	return _mm_add_pd(point.packed(), offset.packed());
}

template<GameVectorType T>
inline GamePoint<T> operator-(const GamePoint<T> &point, const GameVector<T> &offset)
{
	return _mm_sub_pd(point.packed(), offset.packed());
}

template<GameVectorType T>
inline GameVector<T> operator-(const GamePoint<T> &a, const GamePoint<T> &b)
{
	return _mm_sub_pd(a.packed(), b.packed());
}

template<GameVectorType T>
inline bool operator==(const GamePoint<T> &a, const GamePoint<T> &b)
{
	return a.x == b.x && a.y == b.y;
}

template<GameVectorType T>
inline bool operator!=(const GamePoint<T> &a, const GamePoint<T> &b)
{
	return a.x != b.x || a.y != b.y;
}

// 求两点间的距离的平方
template<GameVectorType T>
inline double distance_square(const GamePoint<T> &a, const GamePoint<T> &b)
{
	return (a - b).square();
}

// 求两点间的距离
template<GameVectorType T>
inline double distance(const GamePoint<T> &a, const GamePoint<T> &b)
{
	return (a - b).length();
}

// 求两点的中点
template<GameVectorType T>
inline GamePoint<T> midpoint(const GamePoint<T> &a, const GamePoint<T> &b)
{
	return _mm_mul_pd(_mm_add_pd(a.packed(), b.packed()), { 0.5, 0.5 });
}

// 求靠近点a的第i个n等分点
template<GameVectorType T>
inline GamePoint<T> n_equal_point(const GamePoint<T> &a, const GamePoint<T> &b, qint64 i, qint64 n)
{
	double k = (double)i / (double)n;
	return _mm_add_pd(a.packed(), _mm_mul_pd(_mm_sub_pd(b.packed(), a.packed()), _mm_broadcastsd_pd(_mm_load_sd(&k))));
}

typedef GameVector<MAP_VECTOR> MapVector;
typedef GameVector<SCREEN_VECTOR> ScreenVector;
typedef GamePoint<MAP_VECTOR> MapPoint;
typedef GamePoint<SCREEN_VECTOR> ScreenPoint;

class alignas(16) GameMatrix
{
public:
	GameMatrix() {}

	GameMatrix(const __m256d &src)
	{
		packed() = src;
	}

	GameMatrix(const GameMatrix &src)
	{
		packed() = src.packed();
	}

	GameMatrix(double xx, double xy, double yx, double yy)
		: xx(xx), xy(xy), yx(yx), yy(yy) {}

	// 返回零矩阵
	static GameMatrix Zeros()
	{
		return { 0.0, 0.0, 0.0, 0.0 };
	}

	// 返回单位矩阵
	static GameMatrix Unit()
	{
		return { 1.0, 0.0, 0.0, 1.0 };
	}

	// 两个行向量拼成矩阵
	static GameMatrix RowStack(const GameVectorBase &rx, const GameVectorBase &ry)
	{
		return GameMatrix(rx.x, rx.y, ry.x, ry.y);
	}

	// 两个列向量拼成矩阵
	static GameMatrix ColumnStack(const GameVectorBase &cx, const GameVectorBase &cy)
	{
		return GameMatrix(cx.x, cy.x, cx.y, cy.y);
	}

	// 创建旋转变换矩阵
	static GameMatrix Rotate(double angle)
	{
		double sin_angle = sin(angle), cos_angle = cos(angle);
		return GameMatrix(cos_angle, -sin_angle, sin_angle, cos_angle);
	}

	GameMatrix &operator=(const GameMatrix &src)
	{
		packed() = src.packed();
		return *this;
	}
	GameMatrix &operator+=(const GameMatrix &other)
	{
		packed() = _mm256_add_pd(packed(), other.packed());
		return *this;
	}

	GameMatrix &operator-=(const GameMatrix &other)
	{
		packed() = _mm256_sub_pd(packed(), other.packed());
		return *this;
	}

	GameMatrix &operator*=(double k)
	{
		packed() = _mm256_mul_pd(packed(), _mm256_broadcastsd_pd(_mm_load_sd(&k)));
		return *this;
	}

	GameMatrix &operator/=(double k)
	{
		packed() = _mm256_div_pd(packed(), _mm256_broadcastsd_pd(_mm_load_sd(&k)));
		return *this;
	}

	// 返回该矩阵的转置，不改变自身
	GameMatrix T() const
	{
		return GameMatrix(xx, yx, xy, yy);
	}

	// 求该矩阵的行列式
	double det() const
	{
		return xx * yy - xy * yx;
	}

	// 返回该矩阵的伴随矩阵，不改变自身
	GameMatrix adjoint() const
	{
		return GameMatrix(yy, -xy, -yx, xx);
	}

	// 返回该矩阵的逆矩阵，不改变自身
	GameMatrix reverse() const
	{
		double k = det();
		return _mm256_div_pd({ yy, -xy, -yx, xx }, _mm256_broadcastsd_pd(_mm_load_sd(&k)));
	}

	__m256d &packed() { return *(__m256d*)&xx; }
	const __m256d &packed() const { return *(__m256d*)&xx; }

public:
	double xx, xy, yx, yy;
};

inline GameMatrix operator+(const GameMatrix &A, const GameMatrix &B)
{
	return _mm256_add_pd(A.packed(), B.packed());
}

inline GameMatrix operator-(const GameMatrix &A, const GameMatrix &B)
{
	return _mm256_sub_pd(A.packed(), B.packed());
}

inline GameMatrix operator-(const GameMatrix &A)
{
	return _mm256_sub_pd({ 0.0, 0.0, 0.0, 0.0 }, A.packed());
}

inline GameMatrix operator*(const GameMatrix &A, double k)
{
	return _mm256_mul_pd(A.packed(), _mm256_broadcastsd_pd(_mm_load_sd(&k)));
}

inline GameMatrix operator*(double k, const GameMatrix &A)
{
	return A * k;
}

inline __m128d operator*(const GameMatrix &A, __m128d b)
{
	return _mm_hadd_pd(_mm_mul_pd(*(__m128d*)&A.xx, b), _mm_mul_pd(*(__m128d*)&A.yx, b));
}

template<GameVectorType T>
inline GameVector<T> operator*(const GameMatrix &A, const GameVector<T> &b)
{
	return A * b.packed();
}

inline GameMatrix operator*(const GameMatrix &A, const GameMatrix &B)
{
	__m256d AxxBxx_AxyByx_AyxBxx_AyyByx = _mm256_mul_pd(A.packed(), { B.xx, B.yx, B.xx, B.yx });
	__m256d AxxBxy_AxyByy_AyxBxy_AyyByy = _mm256_mul_pd(A.packed(), { B.xy, B.yy, B.xy, B.yy });
	return _mm256_hadd_pd(AxxBxx_AxyByx_AyxBxx_AyyByx, AxxBxy_AxyByy_AyxBxy_AyyByy);
}

inline GameMatrix operator/(const GameMatrix &A, double k)
{
	return _mm256_div_pd(A.packed(), _mm256_broadcastsd_pd(_mm_load_sd(&k)));
}

inline bool operator==(const GameMatrix &A, const GameMatrix &B)
{
	return A.xx == B.xx && A.xy == B.xy && A.yx == B.yx && A.yy == B.yy;
}

inline bool operator!=(const GameMatrix &A, const GameMatrix &B)
{
	return A.xx != B.xx || A.xy != B.xy || A.yx != B.yx || A.yy != B.yy;
}

template<GameVectorType ST, GameVectorType DT>
class alignas(16) CoordinateTransformation
{
public:
	CoordinateTransformation(__m128d stretch_v, __m128d bias_v)
		: stretch_v(stretch_v), bias_v(bias_v) {}
	CoordinateTransformation(const GameVectorBase &stretch_v, const GameVectorBase &bias_v)
		: stretch_v(stretch_v.packed()), bias_v(bias_v.packed()) {}
	CoordinateTransformation(double stretch_x, double stretch_y, double bias_x, double bias_y)
		: stretch_v{ stretch_x, stretch_y }, bias_v{ bias_x, bias_y } {}

	// 向量变换，只伸缩不平移
	GameVector<DT> operator()(const GameVector<ST> &src) const
	{
		return _mm_mul_pd(src.packed(), stretch_v);
	}

	// 点变换，既伸缩又平移
	GamePoint<DT> operator()(const GamePoint<ST> &src) const
	{
		return _mm_add_pd(_mm_mul_pd(src.packed(), stretch_v), bias_v);
	}

	// 返回它的逆变换，不改变自身
	CoordinateTransformation<DT, ST> reverse() const
	{
		__m128d k_k = _mm_div_pd({ 1.0, 1.0 }, stretch_v);
		__m128d b_b = _mm_mul_pd(k_k, _mm_sub_pd({ 0.0, 0.0 }, bias_v));
		return CoordinateTransformation<DT, ST>(k_k, b_b);
	}

private:
	__m128d stretch_v;
	__m128d bias_v;
};

typedef CoordinateTransformation<MAP_VECTOR, SCREEN_VECTOR> M2S_Transformation;
typedef CoordinateTransformation<SCREEN_VECTOR, MAP_VECTOR> S2M_Transformation;

// 线性变换
class alignas(16) LinearTransformation
{
public:
	LinearTransformation(const GameMatrix &A) : A(A) {}

	// 创建旋转变换对象
	static LinearTransformation Rotate(double angle)
	{
		return GameMatrix::Rotate(angle);
	}

	template<GameVectorType T>
	GameVector<T> operator()(const GameVector<T> &src) const
	{
		return A * src;
	}

	LinearTransformation reverse() const
	{
		return A.reverse();
	}

public:
	GameMatrix A;
};

// 计算先进行线性变换f再进行线性变换g得到的复合变换
inline LinearTransformation compound(const LinearTransformation &f, const LinearTransformation &g)
{
	return LinearTransformation(g.A * f.A);
}

// 仿射变换
class alignas(16) AffineTransformation
{
public:
	AffineTransformation(const GameMatrix &A, __m128d b) : A(A), b(b) {}

	// 创建绕某点的旋转变换对象
	template<GameVectorType T>
	static AffineTransformation Rotate(const GamePoint<T> &p0, double angle)
	{
		AffineTransformation result(GameMatrix::Rotate(angle), p0.packed());
		result.b = (p0 - result.A * GameVector<T>(p0.packed())).packed();
		return result;
	}

	template<GameVectorType T>
	GamePoint<T> operator()(const GamePoint<T> &src) const
	{
		return (A * src + GameVector<T>(b)).packed();
	}

	AffineTransformation reverse() const
	{
		AffineTransformation result(A.reverse(), b);
		result.b = (result.A * -MapVector(b)).packed();
		return result;
	}

public:
	GameMatrix A;
	__m128d b;
};

// 计算先进行仿射变换f再进行线性变换g得到的复合变换
inline AffineTransformation compound(const AffineTransformation &f, const LinearTransformation &g)
{
	return AffineTransformation(g.A * f.A, g.A * f.b);
}

// 计算先进行线性变换f再进行仿射变换g得到的复合变换
inline AffineTransformation compound(const LinearTransformation &f, const AffineTransformation &g)
{
	return AffineTransformation(g.A * f.A, g.b);
}

// 计算先进行变换f再进行仿射变换g得到的复合变换
inline AffineTransformation compound(const AffineTransformation &f, const AffineTransformation &g)
{
	return AffineTransformation(g.A * f.A, _mm_add_pd(g.A * f.b, f.b));
}
