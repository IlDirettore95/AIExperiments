#include "Path.h"
#include <string>

void Path::AddWayPoint(const Vec2& waypoint)
{
	Waypoints.push_back(waypoint);

	if (Waypoints.size() <= 1) return;

	float distance = Vec2::Distance(Waypoints[Waypoints.size() - 2], waypoint);
	float distanceFromStart = Vec2::Distance(waypoint, Waypoints[0]);

	int lastIndex = BetweenWaypointsDistances.size() - 1;
	if (lastIndex < 0)
	{
		// First segment
		BetweenWaypointsDistances.push_back(distance);
		BetweenWaypointsDistances.push_back(distanceFromStart);

		TotalDistances.push_back(distance);
		TotalDistances.push_back(distance + distanceFromStart);

		Length = distance + distanceFromStart;
	}
	else
	{
		// Subsequent segment
		BetweenWaypointsDistances.at(lastIndex) = distance;
		BetweenWaypointsDistances.push_back(distanceFromStart);

		TotalDistances.at(lastIndex) = TotalDistances.at(lastIndex - 1) + distance;
		TotalDistances.push_back(TotalDistances.at(lastIndex) + distanceFromStart);

		Length = TotalDistances.at(lastIndex + 1);
	}
}

void Path::Clear()
{
	Waypoints.clear();
	BetweenWaypointsDistances.clear();
	TotalDistances.clear();
	Length = 0.0f;
	Looped = false;
}

float Path::GetParameter(const Vec2& characterPosition, float lastParameter) const
{
	if (!Looped && lastParameter >= TotalDistances[TotalDistances.size() - 2]) return TotalDistances[TotalDistances.size() - 2];

	int segmentIndex = GetSegment(lastParameter);

	Vec2 start = Waypoints[segmentIndex];
	Vec2 end = Waypoints[(segmentIndex + 1) % Waypoints.size()];

	Vec2 characterPositionDirection = (characterPosition - start);
	Vec2 segmentDirection = (end - start).Normalize();
	float distanceOnSegment = Vec2::Dot(characterPositionDirection, segmentDirection);
	float distanceAtStart = segmentIndex == 0 ? 0 : TotalDistances[segmentIndex - 1];
	float totalDistance = distanceAtStart + distanceOnSegment;

	if (distanceOnSegment < 0) return distanceAtStart;
	
	if (Looped && totalDistance > Length) return totalDistance - Length;
	if (!Looped && totalDistance > Length) return Length;
}

Vec2 Path::GetPosition(float parameter) const
{
	if (Waypoints.size() == 1) return Waypoints[0];
	if (parameter == 0.0f) return Waypoints[0];
	if (!Looped && parameter >= TotalDistances[TotalDistances.size() - 2]) return Waypoints[Waypoints.size() - 1];

	int segmentIndex = GetSegment(parameter);

	Vec2 start = Waypoints[segmentIndex];
	Vec2 end = Waypoints[(segmentIndex + 1) % Waypoints.size()];

	float distanceAtStart = segmentIndex == 0 ? 0 : TotalDistances[segmentIndex - 1];
	float distanceBetweenStartAndEnd = BetweenWaypointsDistances[segmentIndex];

	float t =  (parameter - distanceAtStart) / (distanceBetweenStartAndEnd);

	return start * (1 - t) + end * t;
}

std::string Path::ToString()
{
	std::string string;
	string += "Path:\n";
	string += "\tLength:" + std::to_string(Length) + "\n";
	string += "\tWaypoints:\n";
	for (auto& waypoint : Waypoints)
	{
		string += "\t" + waypoint.ToString() + "\n";
	}
	string += "\tRelativeDistances:\n";
	for (float distance : BetweenWaypointsDistances)
	{
		string += "\t" + std::to_string(distance) + "\n";
	}
	string += "\tTotalDistances:\n";
	for (float distance : TotalDistances)
	{
		string += "\t" + std::to_string(distance) + "\n";
	}
	return string;
}

int Path::GetSegment(float parameter) const
{
	if (parameter > Length) parameter -= Length;

	for (int i = 0; i < TotalDistances.size(); i++)
	{
		if (parameter > TotalDistances[i]) continue;
		else return i; 
	}

	return -1;
}
