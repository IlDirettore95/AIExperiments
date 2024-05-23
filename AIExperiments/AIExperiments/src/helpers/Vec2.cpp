#include "Vec2.h"
#include <math.h>
#include <string>

Vec2::Vec2() {}

Vec2::Vec2(float x, float y)
	: x(x), y(y) {}

bool Vec2::operator==(const Vec2& v) const
{
	return (x == v.x) && (y == v.y);
}

bool Vec2::operator!=(const Vec2& v) const
{
	return (x != v.x) || (y != v.y);
}

Vec2 Vec2::operator+(const Vec2& v) const
{
	return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator*(const float a) const
{
	return Vec2(x * a, y * a);
}

Vec2 Vec2::operator/(const float a) const
{
	return Vec2(x / a, y / a);
}

void Vec2::operator+=(const Vec2& v)
{
	x += v.x;
	y += v.y;
}

void Vec2::operator-=(const Vec2& v)
{
	x -= v.x;
	y -= v.y;
}

void Vec2::operator*=(const float a)
{
	x *= a;
	y *= a;
}

void Vec2::operator/=(const float a)
{
	x /= a;
	y /= a;
}

Vec2 Vec2::operator-(const Vec2& v) const
{
	return Vec2(x - v.x, y - v.y);
}

std::ostream& operator<<(std::ostream& os, Vec2& v)
{
	os << "(" << v.x << " , " << v.y << ")";
	return os;
}
	
Vec2& Vec2::Add(const Vec2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vec2& Vec2::Substract(const Vec2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}
	
Vec2& Vec2::Scale(const float a)
{
	x *= a;
	y *= a;
	return *this;
}
	
float Vec2::Distance(const Vec2& v) const
{
	return sqrtf(powf(x - v.x, 2) + powf(y - v.y, 2));
}
	
float Vec2::Magnitude() const
{
	return sqrtf(powf(x,2) + powf(y,2));
}

Vec2 Vec2::Normalize() const
{
	if (x == 0 && y == 0) return Vec2(0.0f, 0.0f);

	float magnitude = Magnitude();
	return Vec2(x / magnitude, y / magnitude);
}

void Vec2::Print() const
{
	std::cout << x << ", " << y << std::endl;
}

std::string Vec2::ToString() const
{
	std::string string = "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	return string;
}
