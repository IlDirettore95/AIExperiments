#pragma once

#include "helpers/Vec2.h";
#include "core/Components.h"

namespace KinematicMovementsAlgorithms
{
	struct SteeringOutput
	{
		Vec2 Velocity = Vec2(0.0f, 0.0f);
		float Rotation = 0.0f;
	};


	void Update(CStaticData& data, const SteeringOutput& steering);

	float NewOrientation(const float currentOrientation, const Vec2& velocity);

	SteeringOutput Seek(CStaticData& characterData, const CStaticData& targetData, float maxSpeed);	
}

namespace SteeringMovementAlgorithms
{
	struct SteeringOutput
	{
		Vec2 Linear = Vec2(0.0f, 0.0f);
		float Angular = 0.0f;
	};

}