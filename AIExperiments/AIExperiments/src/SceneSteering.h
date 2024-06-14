#pragma once

#include "core/Scene.h"
#include <math.h>

enum class SteeringAlgorithmType
{
	Seek,
	Flee,
	Arrive,
	Wander,
	Max
};

class SceneSteering : public Scene
{
public:
	SceneSteering(GameEngine* gameEngine, const std::string& levelPath);
	void Update();
	void SDoAction(const Action& action);
	void SRenderer();

	void STargetMovement();
	void SSeek();
	void SFlee();
	void SArrive();
	void SWander();

protected:
	void OnEnd();

private:
	Vec2 GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

private:
	const Vec2 m_gridSize = { 32, 32 };
	std::shared_ptr<Entity> m_target = nullptr;
	std::shared_ptr<Entity> m_algorithmDescription = nullptr;
	SteeringAlgorithmType m_algorithmType = SteeringAlgorithmType::Seek;
	bool m_drawGizmos = true;
};
