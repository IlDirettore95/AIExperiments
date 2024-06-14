#pragma once
#include <iostream>

class Vec2
{
public:
	float x = 0;
	float y = 0;

public:

	Vec2();
	Vec2(float x, float y);

	bool operator == (const Vec2& v) const;
	bool operator != (const Vec2& v) const;

	Vec2 operator + (const Vec2& v) const;
	Vec2 operator - (const Vec2& v) const;
	Vec2 operator * (const float a) const;
	Vec2 operator / (const float a) const;

	void operator += (const Vec2& v);
	void operator -= (const Vec2& v);
	void operator *= (const float a);
	void operator /= (const float a);

	friend std::ostream& operator << (std::ostream& os, const Vec2& v);

	Vec2& Add(const Vec2& v);
	Vec2& Substract(const Vec2& v);
	Vec2& Scale(const float a);

	float Distance(const Vec2& v) const;
	float Magnitude() const;

	Vec2 Normalize() const;

	void Print() const;
	std::string ToString() const;
};
