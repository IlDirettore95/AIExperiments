#pragma once
#include "EntityManager.h"
#include <string>
#include "EntityManager.h"
#include <memory>
#include "Action.h"
#include "..\helpers\Color.h"

class GameEngine;

typedef std::map<int, std::string> ActionMap;

class Scene
{
protected:

	GameEngine* m_game = nullptr;
	EntityManager m_entityManager;
	ActionMap m_actionMap;
	size_t m_currentFrame = 0;
	bool m_paused = false;
	bool m_hasEnded = false;

	virtual void OnEnd() = 0;
	void SetPaused(bool paused);

public:

	Scene();
	Scene(GameEngine* gameEngine);

	virtual void Update() = 0;
	virtual void SDoAction(const Action& action) = 0;
	virtual void SRenderer() = 0;

	void DoAction(const Action& action);
	void Simulate(const size_t frames);
	void RegisterAction(int inputKey, const std::string& actionName);

	size_t Width() const;
	size_t Height() const;
	size_t CurrentFrame() const;

	bool HasEnded() const;
	const ActionMap& GetActionMap() const;
	void DrawLine(const Vec2& p1, const Vec2& p2, const Color& color);
};