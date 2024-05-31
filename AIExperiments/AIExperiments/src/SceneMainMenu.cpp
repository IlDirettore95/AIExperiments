#include "SceneMainMenu.h"
#include "core/GameEngine.h"
#include <fstream>
#include <string>
#include "core/Components.h"
#include "SceneKinematic.h"
#include <memory>
#include <filesystem>

#define _USE_MATH_DEFINES
#include <math.h>

SceneMainMenu::SceneMainMenu(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
{
	// Register Action
	{
		RegisterAction(sf::Keyboard::Num1, "KINEMATIC_SEEK");
		RegisterAction(sf::Keyboard::Numpad1, "KINEMATIC_SEEK");
	}

	// Load Level
	{
		// reset the entity manager every time we load a level
		m_entityManager = EntityManager();

		std::fstream fin(levelPath);
		std::string label;

		while (fin >> label)
		{
			if (label == "Text")
			{
				std::string text;
				float posX = 0.0f;
				float posY = 0.0f;

				fin >> text;
				fin >> posX;
				fin >> posY;

				auto textEntity = m_entityManager.AddEntity("Text");
				textEntity->AddComponent<CTransform>(Vec2(posX, posY));
				auto& textComponent = textEntity->AddComponent<CText>(text, m_game->assets().GetFont("FontTech"), 28, sf::Color{255, 255, 255});
			}
		}

	}
}

void SceneMainMenu::Update()
{
	m_entityManager.Update();

	SRenderer();

	m_currentFrame++;
}

void SceneMainMenu::SDoAction(const Action& action)
{
	if (action.Type() == "START")
	{
		if (action.Name() == "KINEMATIC_SEEK") 
		{ 
			std::string scenePath = (std::filesystem::current_path() / "resources" / "kinematic_scenedata.txt").string();
			m_game->changeScene("KINEMATIC_SEEK", std::make_shared<SceneKinematic>(m_game, scenePath));
		}

	}
	else if (action.Type() == "END")
	{

	}
}

void SceneMainMenu::SRenderer()
{
	// color the background darker so you know that the game is paused
	if (!m_paused) { m_game->window().clear(sf::Color(20, 20, 60)); }
	else { m_game->window().clear(sf::Color(0, 0, 30)); }

	// set the viewport of the window to be centered on the player if it's far enough right
	float windowCenterX = m_game->window().getSize().x / 2.0f;
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);

	// draw textures
	for (auto e : m_entityManager.GetEntities())
	{
		auto& transform = e->GetComponent<CTransform>();

		if (e->HasComponent<CAnimation>())
		{
			auto& animation = e->GetComponent<CAnimation>().animation;
			animation.getSprite().setRotation(transform.Static.Orientation * 180.0f / M_PI);
			animation.getSprite().setPosition(transform.Static.Position.x, transform.Static.Position.y);
			animation.getSprite().setScale(transform.scale.x, transform.scale.y);
			m_game->window().draw(animation.getSprite());
		}
	}

	// draw text
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CText>()) continue;
		if (!e->HasComponent<CTransform>()) continue;

		auto& transform = e->GetComponent<CTransform>();
		auto& text = e->GetComponent<CText>().text;
		text.setPosition(transform.Static.Position.x, transform.Static.Position.y);
		m_game->window().draw(text);
	}

	m_game->window().display();
}

void SceneMainMenu::OnEnd()
{
}

Vec2 SceneMainMenu::GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	if (entity->HasComponent<CAnimation>())
	{
		const Vec2& entitySize = entity->GetComponent<CAnimation>().animation.getSize();
		Vec2 entityPosition = Vec2(gridX * m_gridSize.x, gridY * m_gridSize.y) + entitySize / 2;
		entityPosition.y = m_game->window().getSize().y - entityPosition.y;
		return entityPosition;
	}
	else if (entity->HasComponent<CText>())
	{	
		const Vec2& entitySize = Vec2(entity->GetComponent<CText>().text.getLocalBounds().width, entity->GetComponent<CText>().text.getLocalBounds().height);
		Vec2 entityPosition = Vec2(gridX * m_gridSize.x, gridY * m_gridSize.y) + entitySize / 2;
		entityPosition.y = m_game->window().getSize().y - entityPosition.y;
		return entityPosition;
	}
}
