#include "MovementAlgorithms.h"
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <math.h>

Vec2 OrientationAsVector(float currentOrientation)
{

	return Vec2(cosf(currentOrientation), sinf(currentOrientation));
}

float RandomBinomial()
{
	return (((double)rand() / (RAND_MAX)) - ((double)rand() / (RAND_MAX)));
}

namespace KinematicMovementsAlgorithms
{
	void Update(StaticData& data, const SteeringOutput& steering)
	{
		data.Position += steering.Velocity;
		data.Orientation += steering.Rotation;
	}

	float NewOrientation(const float currentOrientation, const Vec2& velocity)
	{
		if (velocity.Magnitude() > 0)
		{
			return atan2f(velocity.y, velocity.x);
		}
		else
		{
			return currentOrientation;
		}
	}

	SteeringOutput Seek(StaticData& characterData, const StaticData& targetData, float maxSpeed)
	{
		SteeringOutput result;

		result.Velocity = targetData.Position - characterData.Position;
		result.Velocity = result.Velocity.Normalize();
		result.Velocity *= maxSpeed;

		characterData.Orientation = NewOrientation(characterData.Orientation, result.Velocity);

		result.Rotation = 0;

		return result;
	}

	SteeringOutput Flee(StaticData& characterData, const StaticData& targetData, float maxSpeed)
	{
		SteeringOutput result;

		result.Velocity = characterData.Position - targetData.Position;
		result.Velocity = result.Velocity.Normalize();
		result.Velocity *= maxSpeed;

		characterData.Orientation = NewOrientation(characterData.Orientation, result.Velocity);

		result.Rotation = 0;

		return result;
	}

	SteeringOutput Arrive(StaticData& characterData, const StaticData& targetData, float maxSpeed, float satisfactionRadius, float timeToTarget)
	{
		SteeringOutput result;

		result.Velocity = targetData.Position - characterData.Position;

		if (result.Velocity.Magnitude() < satisfactionRadius)
		{
			return SteeringOutput();
		}

		result.Velocity /= timeToTarget;

		if (result.Velocity.Magnitude() > maxSpeed)
		{
			result.Velocity = result.Velocity.Normalize() * maxSpeed;
		}

		characterData.Orientation = NewOrientation(characterData.Orientation, result.Velocity);

		result.Rotation = 0;

		return result;
	}

	SteeringOutput Wander(StaticData& characterData, float maxSpeed, float maxRotation)
	{
		SteeringOutput result;

		result.Velocity = OrientationAsVector(characterData.Orientation) * maxSpeed;

		result.Rotation = RandomBinomial() * maxRotation;

		return result;
	}
}


