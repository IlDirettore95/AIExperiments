#pragma once

#include "core/Scene.h"
#include <math.h>
#include "helpers/Path.h"

enum class ESteeringAlgorithmType
{
	Seek,
	Flee,
	Arrive,
	Wander,
	Pursue,
	Evade,
	PathFollowing,
	Max
};

class SceneSteering : public Scene
{
public:
	SceneSteering(GameEngine* gameEngine, const std::string& levelPath);
	void Update();
	void SDoAction(const Action& action);
	void SGui();
	void SRenderer();

	void STargetMovement();
	void SSeek();
	void SFlee();
	void SArrive();
	void SWander();
	void SPursue();
	void SEvade();
	void SPathFollowing();

protected:
	void OnEnd();

private:
	void ChangeBehaviour(ESteeringAlgorithmType newAlgorithm);
	void SpawnWaypoint(const Vec2& position);
	Vec2 GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

private:
	const Vec2 m_gridSize = { 32, 32 };
	std::shared_ptr<Entity> m_ai = nullptr;
	std::shared_ptr<Entity> m_target = nullptr;
	Path m_currentPath;
	std::string m_algorithmTypeDescription;
	std::string m_algorithmDescription;
	ESteeringAlgorithmType m_algorithmType = ESteeringAlgorithmType::Seek;
	bool m_drawGizmos = false;
	bool m_followPath = false;
	sf::Clock m_deltaClock;
};
