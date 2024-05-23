#include "MovementAlgorithms.h"

namespace KinematicMovementsAlgorithms
{
	void Update(CStaticData& data, const SteeringOutput& steering)
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

	SteeringOutput Seek(CStaticData& characterData, const CStaticData& targetData, float maxSpeed)
	{
		SteeringOutput result;

		result.Velocity = targetData.Position - characterData.Position;
		result.Velocity = result.Velocity.Normalize();
		result.Velocity *= maxSpeed;

		characterData.Orientation = NewOrientation(characterData.Orientation, result.Velocity);

		result.Rotation = 0;

		return result;
	}
}