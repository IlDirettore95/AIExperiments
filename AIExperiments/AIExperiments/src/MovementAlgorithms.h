#pragma once

#include "helpers/Vec2.h"
#include "core/Components.h"
#include "core/Scene.h"

Vec2 OrientationAsVector(float currentOrientation);

float VectorAsOrientation(Vec2 vector);

float RandomBinomial();

float MapToRange(float orientation);

namespace KinematicMovementsAlgorithms
{
	struct SteeringOutput
	{
		Vec2 Velocity = { 0.0f, 0.0f };
		float AngularVelocity = 0.0f;
	};

	void Update(StaticData& data, const SteeringOutput& steering);

	float NewOrientation(const float currentOrientation, const Vec2& velocity);

	SteeringOutput Seek(StaticData& characterData, const StaticData& targetData, float maxSpeed);	

	SteeringOutput Flee(StaticData& characterData, const StaticData& targetData, float maxSpeed);

	SteeringOutput Arrive(StaticData& characterData, const StaticData& targetData, float maxSpeed, float satisfactionRadius, float timeToTarget);

	SteeringOutput Wander(StaticData& characterData, float maxSpeed, float maxRotation);
}

namespace SteeringMovementsAlgorithms
{
	struct SteeringOutput
	{
		Vec2 Linear = { 0.0f, 0.0f };
		float Angular = 0.0f;
	};

	void Update(StaticData& staticData, DynamicData& dynamicData, const SteeringOutput& steering, const CSteeringAI& characterSteering);

	SteeringOutput Seek(const StaticData& characterData, const StaticData& targetData, const CSteeringAI& characterSteering);

	SteeringOutput Flee(const StaticData& characterData, const StaticData& targetData, const CSteeringAI& characterSteering);

	SteeringOutput Arrive(const StaticData& characterStaticData, const DynamicData& characterDynamicData, const StaticData& targetData, const CSteeringAI& characterSteering);

	SteeringOutput Align(const StaticData& characterStaticData, const DynamicData& characterDynamicData, const StaticData& targetData, const CSteeringAI& characterSteering);

	SteeringOutput LookWhereYouAreGoing(const StaticData& characterStaticData, const DynamicData& characterDynamicData, const CSteeringAI& characterSteering);

	SteeringOutput Face(const StaticData& characterStaticData, const DynamicData& characterDynamicData, const StaticData& targetData, const CSteeringAI& characterSteering);

	SteeringOutput Wander(const StaticData& characterStaticData, const DynamicData& characterDynamicData, CSteeringAI& characterSteering);
}