#pragma once

#include "../helpers/Vec2.h"
#include <vector>

class Path
{
public:
	std::vector<Vec2> Waypoints;
	std::vector<float> BetweenWaypointsDistances;
	std::vector<float> TotalDistances;
	float Length = 0;
	bool Looped = false;

	void AddWayPoint(const Vec2& waypoint);
	void Clear();
	float GetParameter(const Vec2& characterPosition, float lastParameter) const;
	Vec2 GetPosition(float parameter) const;

	std::string ToString();

private:
	int GetSegment(float parameter) const;
};
