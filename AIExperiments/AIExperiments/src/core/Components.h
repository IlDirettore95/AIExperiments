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

class CTransform : public Component
{
public:
	StaticData Static;
	Vec2 velocity = { 0.0, 0.0 };
	Vec2 scale = { 1.0, 1.0 };
	Vec2 prevPos = { 0.0, 0.0 };

	CTransform() {}
	CTransform(const Vec2& p)
	{
		Static.Position = p;
	}
	CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
		: prevPos(p), velocity(sp), scale(sc) 
	{
		Static.Position = p;
		Static.Orientation = a;
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
	sf::Text text;

	CText() {}
	CText(const std::string& name, const sf::Font& font, const uint32_t fontSize, const sf::Color& color)
		: text(name, font, fontSize)
	{
		text.setFillColor(color);
		text.setOrigin(text.getLocalBounds().width / 2 + text.getLocalBounds().left, text.getLocalBounds().height / 2 + text.getLocalBounds().top);
	}
};

struct CTargetToSeek : public Component
{
	size_t EntityID;
	float MaxSpeed = 1.0f;
};