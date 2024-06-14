#include "MovementAlgorithms.h"
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <math.h>

Vec2 OrientationAsVector(float currentOrientation)
{

	return Vec2(cosf(currentOrientation), sinf(currentOrientation));
}

float VectorAsOrientation(Vec2 vector)
{
	return atan2f(vector.y, vector.x);
}

float RandomBinomial()
{
	return (((double)rand() / (RAND_MAX)) - ((double)rand() / (RAND_MAX)));
}

float MapToRange(float orientation)
{
	while (orientation > M_PI)
	{
		orientation -= (2 * M_PI);
	}

	while (orientation < -M_PI)
	{
		orientation += (2 * M_PI);
	}

	return orientation;
}

namespace KinematicMovementsAlgorithms
{
	void Update(StaticData& data, const SteeringOutput& steering)
	{
		data.Position += steering.Velocity;
		data.Orientation += steering.AngularVelocity;
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

		result.AngularVelocity = 0;

		return result;
	}

	SteeringOutput Flee(StaticData& characterData, const StaticData& targetData, float maxSpeed)
	{
		SteeringOutput result;

		result.Velocity = characterData.Position - targetData.Position;
		result.Velocity = result.Velocity.Normalize();
		result.Velocity *= maxSpeed;

		characterData.Orientation = NewOrientation(characterData.Orientation, result.Velocity);

		result.AngularVelocity = 0;

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

		result.AngularVelocity = 0;

		return result;
	}

	SteeringOutput Wander(StaticData& characterData, float maxSpeed, float maxRotation)
	{
		SteeringOutput result;

		result.Velocity = OrientationAsVector(characterData.Orientation) * maxSpeed;

		result.AngularVelocity = RandomBinomial() * maxRotation;

		return result;
	}
}

namespace SteeringMovementsAlgorithms
{

	void Update(StaticData& staticData, DynamicData& dynamicData, const SteeringOutput& steering, const CSteeringAI& characterSteering)
	{
		staticData.Position += dynamicData.Velocity;
		staticData.Orientation += dynamicData.Angular;

		dynamicData.Velocity += steering.Linear;
		dynamicData.Angular += steering.Angular;

		if (dynamicData.Velocity.Magnitude() > characterSteering.MaxSpeed)
		{
			dynamicData.Velocity = dynamicData.Velocity.Normalize() * characterSteering.MaxSpeed;
		}

		if (std::abs(dynamicData.Angular) > characterSteering.MaxAngularSpeed)
		{
			dynamicData.Angular /= std::abs(dynamicData.Angular) * characterSteering.MaxAngularSpeed;
		}
	}

	SteeringOutput Seek(const StaticData& characterData, const StaticData& targetData, const CSteeringAI& characterSteering)
	{
		SteeringOutput result;

		result.Linear = targetData.Position - characterData.Position;
		result.Linear = result.Linear.Normalize() * characterSteering.MaxAcceleration;
		result.Angular = 0.0f;

		return result;
	}

	SteeringOutput Flee(const StaticData& characterData, const StaticData& targetData, const CSteeringAI& characterSteering)
	{
		SteeringOutput result;

		result.Linear = characterData.Position - targetData.Position;
		result.Linear = result.Linear.Normalize() * characterSteering.MaxAcceleration;
		result.Angular = 0.0f;

		return result;
	}

	SteeringOutput Arrive(const StaticData& characterStaticData, const DynamicData& characterDynamicData, const StaticData& targetData, const CSteeringAI& characterSteering)
	{
		SteeringOutput result;

		Vec2 direction = targetData.Position - characterStaticData.Position;
		float distance = direction.Magnitude();
		if (distance <= characterSteering.DistanceTargetRadius)
		{
			return result;
		}

		float targetSpeed = 0.0f;
		if (distance > characterSteering.DistanceSlowRadius)
		{
			targetSpeed = characterSteering.MaxSpeed;
		}
		else
		{
			targetSpeed = characterSteering.MaxSpeed * (distance - characterSteering.DistanceTargetRadius) / (characterSteering.DistanceSlowRadius - characterSteering.DistanceTargetRadius);
		}

		Vec2 targetVelocity = direction.Normalize() * targetSpeed;

		result.Linear = targetVelocity - characterDynamicData.Velocity;

		if (result.Linear.Magnitude() > characterSteering.MaxAcceleration)
		{
			result.Linear = result.Linear.Normalize() * characterSteering.MaxAcceleration;
		}
		result.Angular = 0.0f;

		return result;
	}

	SteeringOutput Align(const StaticData& characterStaticData, const DynamicData& characterDynamicData, const StaticData& targetData, const CSteeringAI& characterSteering)
	{
		SteeringOutput result;

		float angularDirection = targetData.Orientation - characterStaticData.Orientation;
		angularDirection = MapToRange(angularDirection);
		float angularDistance = std::abs(angularDirection);
		if (angularDistance <= characterSteering.OrientationTargetRadius)
		{
			return result;
		}

		float targetAngularSpeed = 0.0f;
		if (angularDistance > characterSteering.OrientationSlowRadius)
		{
			targetAngularSpeed = characterSteering.MaxAngularSpeed;
		}
		else
		{
			targetAngularSpeed = characterSteering.MaxAngularSpeed * angularDistance / characterSteering.OrientationSlowRadius;
		}

		// Multiply for 1 or -1 to reintroduce the direction
		targetAngularSpeed *= angularDirection / angularDistance;

		result.Angular = targetAngularSpeed - characterDynamicData.Angular;

		float angularAcceleration = std::abs(result.Angular);
		if (angularAcceleration > characterSteering.MaxAngularAcceleration)
		{
			result.Angular /= angularAcceleration;
			result.Angular *= characterSteering.MaxAngularAcceleration;
		}

		result.Linear = Vec2();

		return result;
	}

	SteeringOutput LookWhereYouAreGoing(const StaticData& characterStaticData, const DynamicData& characterDynamicData, const CSteeringAI& characterSteering)
	{
		Vec2 velocity = characterDynamicData.Velocity;

		if (velocity.Magnitude() == 0)
		{
			return SteeringOutput();
		}
		
		StaticData targetData;
		targetData.Orientation = VectorAsOrientation(velocity); 

		return Align(characterStaticData, characterDynamicData, targetData, characterSteering);
	}

	SteeringOutput Face(const StaticData& characterStaticData, const DynamicData& characterDynamicData, const StaticData& targetData, const CSteeringAI& characterSteering)
	{
		Vec2 direction = targetData.Position - characterStaticData.Position;
		if (direction.Magnitude() == 0.0f)
		{
			return SteeringOutput();
		}

		StaticData newTargetData = targetData;
		newTargetData.Orientation = VectorAsOrientation(direction.Normalize());
		return Align(characterStaticData, characterDynamicData, newTargetData, characterSteering);
	}

	SteeringOutput Wander(const StaticData& characterStaticData, const DynamicData& characterDynamicData, CSteeringAI& characterSteering)
	{
		SteeringOutput result;

		characterSteering.WanderOrientation += RandomBinomial() * characterSteering.WanderRate;

		float targetOrientation = characterSteering.WanderOrientation + characterStaticData.Orientation;
		Vec2 targetPosition = characterStaticData.Position + OrientationAsVector(characterStaticData.Orientation) * characterSteering.WanderOffset;
		targetPosition += OrientationAsVector(targetOrientation) * characterSteering.WanderRadius;

		StaticData targetData;
		targetData.Position = targetPosition;
		targetData.Orientation = targetOrientation;

		result = Face(characterStaticData, characterDynamicData, targetData, characterSteering);
		
		result.Linear = OrientationAsVector(characterStaticData.Orientation) * characterSteering.MaxAcceleration;

		return result;
	}
}
