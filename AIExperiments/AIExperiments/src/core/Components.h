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

	Vec2 Scale = { 1.0, 1.0 };

	CTransform() {}
	CTransform(const Vec2& p)
	{
		Static.Position = p;
	}
	CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
		: Scale(sc) 
	{
		Static.Position = p;
		Static.Orientation = a;
		Dynamic.Velocity = sp;
	}
	CTransform(const CTransform& transform)
	{
		Static = transform.Static;
		Dynamic = transform.Dynamic;
		Scale = transform.Scale;
	}
};

class CAnimation : public Component
{
public:
	Animation AnimationField;
	bool Repeat = false;

	CAnimation() {}
	CAnimation(Animation animation, bool r)
		: AnimationField(animation), Repeat(r) {}
	CAnimation(const CAnimation& animationComponent)
	{
		AnimationField = animationComponent.AnimationField;
		Repeat = animationComponent.Repeat;
	}
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
	CText(const CText& textComponent)
	{
		text = textComponent.text;
	}

};

class CSteeringAI : public Component
{
public:
	size_t EntityID = UINT64_MAX;						// Entity Target
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
	float MaxPreditionTime = 1.0f;

	CSteeringAI() {}
	CSteeringAI(const CSteeringAI& steeringComponent)
	{
		EntityID				= steeringComponent.EntityID;
		MaxSpeed				= steeringComponent.MaxSpeed;
		MaxAcceleration			= steeringComponent.MaxAcceleration;
		MaxAngularSpeed			= steeringComponent.MaxAngularSpeed;
		MaxAngularAcceleration	= steeringComponent.MaxAngularAcceleration;
		DistanceSlowRadius		= steeringComponent.DistanceSlowRadius;
		DistanceTargetRadius	= steeringComponent.DistanceTargetRadius;
		OrientationSlowRadius	= steeringComponent.OrientationSlowRadius;
		OrientationTargetRadius = steeringComponent.OrientationTargetRadius;
		WanderOffset			= steeringComponent.WanderOffset;
		WanderRadius			= steeringComponent.WanderRadius;
		WanderRate				= steeringComponent.WanderRate;
		WanderOrientation		= steeringComponent.WanderOrientation;
		MaxPreditionTime		= steeringComponent.MaxPreditionTime;
	}
};

class CFollowMouse : public Component
{
public:
	sf::CircleShape debugCircle;

	CFollowMouse() {}
	CFollowMouse(const CFollowMouse& followMouseComponent)
	{
		debugCircle = followMouseComponent.debugCircle;
	}
};