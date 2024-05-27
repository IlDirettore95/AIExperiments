#include "SceneKinematicSeek.h"
#include "core/GameEngine.h"
#include <fstream>
#include <string>
#include "core/Components.h"
#include "MovementAlgorithms.h"
#include <filesystem>
#include "SceneMainMenu.h"

#define _USE_MATH_DEFINES
#include <math.h>

SceneKinematicSeek::SceneKinematicSeek(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
{
	// Register Action
	{
		RegisterAction(sf::Keyboard::Escape, "BACK_TO_MAIN_MENU");
	}

	// Load Level
	{
		// reset the entity manager every time we load a level
		m_entityManager = EntityManager();

		// Spawn a Target Entity
		auto target = m_entityManager.AddEntity("TargetEntity");
		target->AddComponent<CTransform>();

		std::fstream fin(levelPath);
		std::string label;

		while (fin >> label)
		{
			if (label == "AICharacter")
			{
				std::string animationName;
				float posX = 0.0f;
				float posY = 0.0f;

				fin >> animationName;
				fin >> posX;
				fin >> posY;

				auto aiCharacter = m_entityManager.AddEntity("AICharacter");
				aiCharacter->AddComponent<CAnimation>(m_game->assets().GetAnimation(animationName), true);
				aiCharacter->AddComponent<CTransform>(GridToMidPixel(posX, posY, aiCharacter));
				CTargetToSeek& targetToSeek = aiCharacter->AddComponent<CTargetToSeek>();
				targetToSeek.EntityID = target->id();
				targetToSeek.MaxSpeed = 2.0f;
			}
		}

	}
}

void SceneKinematicSeek::Update()
{
	m_entityManager.Update();

	SSeek();
	SRenderer();

	m_currentFrame++;
}

void SceneKinematicSeek::SDoAction(const Action& action)
{
	if (action.Type() == "START")
	{
		if (action.Name() == "BACK_TO_MAIN_MENU")
		{
			std::string scenePath = (std::filesystem::current_path() / "resources" / "main_menu_scenedata.txt").string();
			m_game->changeScene("MAIN_MENU", std::make_shared<SceneMainMenu>(m_game, scenePath));
		}
	}
	else if (action.Type() == "END")
	{

	}
}

void SceneKinematicSeek::SRenderer()
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


	m_game->window().display();
}

void SceneKinematicSeek::SSeek()
{
	// Move Target
	for (auto e : m_entityManager.GetEntities("TargetEntity"))
	{
		if (!e->HasComponent<CTransform>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		transform.Static.Position = Vec2(sf::Mouse::getPosition(m_game->window()).x, sf::Mouse::getPosition(m_game->window()).y);
	}

	// Seek
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CTargetToSeek>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CTargetToSeek& targetToSeek = e->GetComponent<CTargetToSeek>();

		for (auto otherE : m_entityManager.GetEntities())
		{
			if (e == otherE) continue;
			if (otherE->id() == targetToSeek.EntityID)
			{
				if (otherE->HasComponent<CTransform>())
				{
					const CTransform& targetTransform = otherE->GetComponent<CTransform>();
					KinematicMovementsAlgorithms::SteeringOutput steering = KinematicMovementsAlgorithms::Seek(transform.Static, targetTransform.Static, targetToSeek.MaxSpeed);
					KinematicMovementsAlgorithms::Update(transform.Static, steering);

					//std::cout << steering.Velocity.Magnitude() << std::endl;
					//std::cout << transform.Static.Orientation << std::endl;
				}
				break;
			}
		}
	}
}

void SceneKinematicSeek::OnEnd()
{
}

Vec2 SceneKinematicSeek::GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	const Vec2& entitySize = entity->GetComponent<CAnimation>().animation.getSize();
	Vec2 entityPosition = Vec2(gridX * m_gridSize.x, gridY * m_gridSize.y) + entitySize / 2;
	entityPosition.y = m_game->window().getSize().y - entityPosition.y;
	return entityPosition;
}