#pragma once

#include "helpers/Vec2.h";
#include "core/Components.h"

Vec2 OrientationAsVector(float currentOrientation);

float RandomBinomial();

namespace KinematicMovementsAlgorithms
{
	struct SteeringOutput
	{
		Vec2 Velocity = Vec2(0.0f, 0.0f);
		float Rotation = 0.0f;
	};


	void Update(StaticData& data, const SteeringOutput& steering);

	float NewOrientation(const float currentOrientation, const Vec2& velocity);

	SteeringOutput Seek(StaticData& characterData, const StaticData& targetData, float maxSpeed);	

	SteeringOutput Flee(StaticData& characterData, const StaticData& targetData, float maxSpeed);

	SteeringOutput Arrive(StaticData& characterData, const StaticData& targetData, float maxSpeed, float satisfactionRadius, float timeToTarget);

	SteeringOutput Wander(StaticData& characterData, float maxSpeed, float maxRotation);
}

namespace SteeringMovementAlgorithms
{
	struct SteeringOutput
	{
		Vec2 Linear = Vec2(0.0f, 0.0f);
		float Angular = 0.0f;
	};

}