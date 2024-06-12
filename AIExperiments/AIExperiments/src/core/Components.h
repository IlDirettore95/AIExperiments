#pragma once

#include "Animation.h"
#include "Assets.h"
#include <memory>

class Component
{
public:
	bool has = false;
};

struct StaticData
{
	Vec2 Position = { 0.0f, 0.0f };
	float Orientation = 0.0f;
};

struct DynamicData
{
	Vec2 Velocity = { 0.0f, 0.0f };
	float Angular = 0.0f;
};

class CTransform : public Component
{
public:
	StaticData Static;
	DynamicData Dynamic;

	Vec2 scale = { 1.0, 1.0 };
	Vec2 prevPos = { 0.0, 0.0 };

	CTransform() {}
	CTransform(const Vec2& p)
	{
		Static.Position = p;
	}
	CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
		: prevPos(p), scale(sc) 
	{
		Static.Position = p;
		Static.Orientation = a;
		Dynamic.Velocity = sp;
	}
};

class CLifeSpan : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;

	CLifeSpan() {}
	CLifeSpan(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;

	CBoundingBox() {}
	CBoundingBox(const Vec2& s)
		: size(s), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;

	CAnimation() {}
	CAnimation(const Animation& animation, bool r)
		: animation(animation), repeat(r) {}
};

class CGravity : public Component
{
public:
	float gravity = 0;
	CGravity() {}
	CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
public:
	std::string state = "jumping";

	CState() {}
	CState(const std::string& s)
		: state(s) {}
};

class CText : public Component
{
public:
	enum class AlignType
	{
		Left,
		Center,
		Right
	};

public:
	sf::Text text;

	CText() {}
	CText(const std::string& name, const sf::Font& font, const uint32_t fontSize, const sf::Color& color, AlignType alignment)
		: text(name, font, fontSize)
	{
		text.setFillColor(color);

		switch (alignment)
		{
		case CText::AlignType::Left:
			{
				text.setOrigin(text.getLocalBounds().left, text.getLocalBounds().height / 2 + text.getLocalBounds().top);
			}
			break;

			case CText::AlignType::Center:
			{
				text.setOrigin(text.getLocalBounds().width / 2 + text.getLocalBounds().left, text.getLocalBounds().height / 2 + text.getLocalBounds().top);
			}
			break;

			case CText::AlignType::Right:
			{
				text.setOrigin(text.getLocalBounds().width + text.getLocalBounds().left, text.getLocalBounds().height / 2 + text.getLocalBounds().top);
			}
			break;
		}
	}

};

class CSteeringAI : public Component
{
public:
	size_t EntityID;						// Entity Target
	float MaxSpeed = 1.0f;		
	float MaxAcceleration = 1.0f;
	float MaxAngularSpeed = 1.0f;
	float MaxAngularAcceleration = 1.0f;
	float DistanceSlowRadius = 1.0f;
	float DistanceTargetRadius = 1.0f;
	float OrientationSlowRadius = 1.0f;
	float OrientationTargetRadius = 1.0f;
	float WanderOffset = 1.0f;
	float WanderRadius = 1.0f;
	float WanderRate = 1.0f;
	float WanderOrientation = 0.0f;
};

class CFollowMouse : public Component
{

};

// Define a shape to be rendered
class CCircleShape : public Component
{
public:
	sf::CircleShape shape;
	float radius = 1.0f;
	int points = 32;

	CCircleShape() {}
	CCircleShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: shape(radius, points), radius(radius), points(points)
	{
		shape.setFillColor(fill);
		shape.setOutlineColor(outline);
		shape.setOutlineThickness(thickness);
		shape.setOrigin(radius, radius);
	}
};