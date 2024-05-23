#pragma once
#include "core/Scene.h"
#include <map>
#include <string>
#include <memory>
#include "core/EntityManager.h"

class Scene_Play : public Scene
{
	struct PlayerConfig
	{
		float X, Y, CX, CY, ACCELERATION, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

protected:

	std::shared_ptr<Entity> m_player;
	std::string m_levelPath;
	PlayerConfig m_playerConfig;
	bool m_drawTextures = true;
	bool m_drawCollision = false;
	bool m_drawGrid = false;
	const Vec2 m_gridSize = { 64, 64 };
	sf::Text m_gridText;

	void Init(const std::string& levelPath);

	Vec2 GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

	void LoadLevel(const std::string& filename);

	void SpawnPlayer();

	void SpawnExplosion(const Vec2& spawnPosition);

	void SpawnBullet();

	void OnEnd();

public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);

	void Update();

	// Systems
	void SAnimation();
	void SMovement();
	void SEnemySpawner();
	void SCollision();
	void SRenderer();
	void SDoAction(const Action& action);
	void SDebug();
	void SLifespan();
};