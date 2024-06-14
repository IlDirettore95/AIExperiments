#include "Scene.h"
#include "GameEngine.h"
#include <iostream>

void Scene::SetPaused(bool paused)
{
	m_paused = paused;
}

Scene::Scene() {}

Scene::Scene(GameEngine* gameEngine)
	: m_game(gameEngine) {}

void Scene::DoAction(const Action& action)
{

}

void Scene::Simulate(const size_t frames)
{
	for (int i = 0; i < frames; i++)
	{
		Update();
	}
}

void Scene::RegisterAction(int inputKey, const std::string& actionName)
{
	m_actionMap[inputKey] = actionName;
}

size_t Scene::Width() const
{
	return m_game->window().getSize().x;
}

size_t Scene::Height() const
{
	return m_game->window().getSize().y;
}

size_t Scene::CurrentFrame() const
{
	return m_currentFrame;
}

bool Scene::HasEnded() const
{
	return m_hasEnded;
}

const ActionMap& Scene::GetActionMap() const
{
	return m_actionMap;
}

void Scene::DrawLine(const Vec2& start, const Vec2& end, const Color& color)
{
	sf::Color sfColor(color.r * 255, color.g * 255, color.b * 255, 255);
	sf::Vertex line[] = { sf::Vertex(sf::Vector2f(start.x, start.y), sfColor), sf::Vertex(sf::Vector2f(end.x, end.y), sfColor) };
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene::DrawCircle(const Vec2& center, float radius, int points, const Color& fillColor, const Color& outlineColor, float thickness)
{
	sf::Color sfFillColor(fillColor.r * 255, fillColor.g * 255, fillColor.b * 255, 255);
	sf::Color sfOutlineColor(outlineColor.r * 255, outlineColor.g * 255, outlineColor.b * 255, 255);

	sf::CircleShape sfCircleShape;
	sfCircleShape.setRadius(radius);
	sfCircleShape.setOrigin(radius, radius);
	sfCircleShape.setPointCount(points);
	sfCircleShape.setFillColor(sfFillColor);
	sfCircleShape.setOutlineColor(sfOutlineColor);
	sfCircleShape.setOutlineThickness(thickness);
	sfCircleShape.setPosition(center.x, center.y);

	m_game->window().draw(sfCircleShape);
}
