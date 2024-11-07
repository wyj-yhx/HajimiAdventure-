#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

class Vector2
{
public:
	float x = 0;
	float y = 0;

public:
	Vector2() = default;
	~Vector2() = default;

    Vector2(float x, float y) : x(x), y(y) {}

	Vector2 operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	void operator+=(const Vector2& other)
	{
        x += other.x;
        y += other.y;
	}

	void operator-=(const Vector2& other)
	{
        x -= other.x;
        y -= other.y;
	}

	Vector2 operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}

	float operator*(const Vector2& other) const
	{
		return x * other.x + y * other.y;
	}

	Vector2 operator*(float val) const
	{
		return Vector2(x * val, y * val);
	}

	void operator*=(float val)
	{
		x *= val;
		y *= val;
	}

	Vector2 operator/(float val) const
	{
		return Vector2(x / val, y / val);
	}

	void operator/=(float val)
	{
		x /= val;
		y /= val;
	}

	float length()
	{
		return sqrt(x * x + y * y);
	}

	//单位向量计算
	Vector2 normalize()
	{
		float len = length();
		if(len == 0)
			return Vector2(0, 0);
        return Vector2(x / len, y / len);
	}

	//判断两个向量是否近似相等
	bool approx(const Vector2& other)
	{
		return (*this - other).length() <= 0.0001;
	}
};











#endif // VECTOR2_H