#pragma once

#include "core/Scene.h"
#include <math.h>

class SceneKinematicSeek : public Scene
{
public:
	SceneKinematicSeek(GameEngine* gameEngine, const std::string& levelPath);
	void Update();
	void SDoAction(const Action& action);
	void SRenderer();

	void SSeek();
protected:
	void OnEnd();
private:
	Vec2 GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

private:
	const Vec2 m_gridSize = { 32, 32 };
};
