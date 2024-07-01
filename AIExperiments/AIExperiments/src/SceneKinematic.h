#pragma once

#include "core/Scene.h"
#include <math.h>

enum class EKinematicAlgorithmType
{
	Seek,
	Flee,
	Arrive,
	Wander,
	Max
};

class SceneKinematic : public Scene
{
public:
	SceneKinematic(GameEngine* gameEngine, const std::string& levelPath);
	void Update();
	void SDoAction(const Action& action);
	void SGui();
	void SRenderer();

	void STargetMovement();
	void SSeek();
	void SFlee();
	void SArrive();
	void SWander();

protected:
	void OnEnd();

private:
	void ChangeBehaviour(EKinematicAlgorithmType newAlgorithm);
	Vec2 GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

private:
	const Vec2 m_gridSize = { 32, 32 };
	std::shared_ptr<Entity> m_ai = nullptr;
	std::shared_ptr<Entity> m_target = nullptr;
	std::string m_algorithmTypeDescription;
	std::string m_algorithmDescription;
	EKinematicAlgorithmType m_algorithmType = EKinematicAlgorithmType::Seek;
	bool m_drawGizmos = false;
	sf::Clock m_deltaClock;
};
