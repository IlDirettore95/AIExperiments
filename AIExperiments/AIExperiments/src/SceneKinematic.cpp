#include "SceneKinematic.h"
#include "core/GameEngine.h"
#include <fstream>
#include <string>
#include "core/Components.h"
#include "MovementAlgorithms.h"
#include <filesystem>
#include "SceneMainMenu.h"
#include <cstdlib>

#define _USE_MATH_DEFINES
#include <math.h>

SceneKinematic::SceneKinematic(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
{
	// SetRandomSeed	
	srand(time(NULL));

	// Register Action
	{
		RegisterAction(sf::Keyboard::Escape, "BACK_TO_MAIN_MENU");
		RegisterAction(sf::Keyboard::Space, "SWITCH_ALGORITHM");
	}

	// Load Level
	{
		// reset the entity manager every time we load a level
		m_entityManager = EntityManager();

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
				CSteeringAI& targetToSeek = aiCharacter->AddComponent<CSteeringAI>();
				targetToSeek.EntityID = m_target->id();
				targetToSeek.MaxSpeed = 2.0f;
			}
			else if (label == "MovingTarget")
			{
				m_target = m_entityManager.AddEntity("MovingTarget");
				m_target->AddComponent<CTransform>();
				m_target->AddComponent<CFollowMouse>();
				m_target->AddComponent<CCircleShape>(10.0f, 20, sf::Color::Transparent, sf::Color::Green, 1.0f);
			}
			else if (label == "TextAlgorithmType")
			{
				std::string text;
				float posX = 0.0f;
				float posY = 0.0f;
				std::string alignment;

				fin >> text;
				fin >> posX;
				fin >> posY;
				fin >> alignment;

				CText::AlignType alignmentType = CText::AlignType::Left;

				if (alignment == "Left")
				{
					alignmentType = CText::AlignType::Left;
				}
				else if (alignment == "Center")
				{
					alignmentType = CText::AlignType::Center;
				}
				else if (alignment == "Right")
				{
					alignmentType = CText::AlignType::Right;
				}

				m_algorithmDescription = m_entityManager.AddEntity("Text");
				m_algorithmDescription->AddComponent<CTransform>(Vec2(posX, posY));
				auto& textComponent = m_algorithmDescription->AddComponent<CText>(text, m_game->assets().GetFont("FontTech"), 28, sf::Color{ 255, 255, 255 }, alignmentType);
				textComponent.text.setString("Algorithm: Seek");
			}
			else if (label == "Text")
			{
				std::string text;
				float posX = 0.0f;
				float posY = 0.0f;
				std::string alignment;

				fin >> text;
				fin >> posX;
				fin >> posY;
				fin >> alignment;

				CText::AlignType alignmentType = CText::AlignType::Left;

				if (alignment == "Left")
				{
					alignmentType = CText::AlignType::Left;
				}
				else if (alignment == "Center")
				{
					alignmentType = CText::AlignType::Center;
				}
				else if (alignment == "Right")
				{
					alignmentType = CText::AlignType::Right;
				}

				auto textEntity = m_entityManager.AddEntity("Text");
				textEntity->AddComponent<CTransform>(Vec2(posX, posY));
				auto& textComponent = textEntity->AddComponent<CText>(text, m_game->assets().GetFont("FontTech"), 28, sf::Color{ 255, 255, 255 }, alignmentType);
			}
		}

	}
}

void SceneKinematic::Update()
{
	m_entityManager.Update();

	STargetMovement();

	switch (m_algorithmType)
	{
		case KinematicAlgorithmType::Seek:
		{
			SSeek();
			break;
		}

		case KinematicAlgorithmType::Flee:
		{
			SFlee();
			break;
		}

		case KinematicAlgorithmType::Arrive:
		{
			SArrive();
			break;
		}

		case KinematicAlgorithmType::Wander:
		{
			SWander();
			break;
		}
	}

	SRenderer();

	m_currentFrame++;
}

void SceneKinematic::SDoAction(const Action& action)
{
	if (action.Type() == "START")
	{
		if (action.Name() == "BACK_TO_MAIN_MENU")
		{
			std::string scenePath = (std::filesystem::current_path() / "resources" / "main_menu_scenedata.txt").string();
			m_game->changeScene("MAIN_MENU", std::make_shared<SceneMainMenu>(m_game, scenePath));
		}
		else if (action.Name() == "SWITCH_ALGORITHM")
		{
			m_algorithmType = (KinematicAlgorithmType)(((uint8_t)m_algorithmType + 1) % ((uint8_t)KinematicAlgorithmType::Max));
			switch (m_algorithmType)
			{
				case KinematicAlgorithmType::Seek:
				{
					m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Seek");
					break;
				}

				case KinematicAlgorithmType::Flee:
				{
					m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Flee");
					break;
				}

				case KinematicAlgorithmType::Arrive:
				{
					m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Arrive");
					break;
				}

				case KinematicAlgorithmType::Wander:
				{
					m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Wander");
					break;
				}
			}
		}
	}
	else if (action.Type() == "END")
	{

	}
}

void SceneKinematic::SRenderer()
{
	// turn off cursor
	m_game->window().setMouseCursorVisible(false);

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
			animation.getSprite().setRotation(transform.Static.Orientation * (180.0f / M_PI));
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

		const auto& transform = e->GetComponent<CTransform>();
		auto& text = e->GetComponent<CText>().text;
		text.setPosition(transform.Static.Position.x, transform.Static.Position.y);
		m_game->window().draw(text);
	}

	// Draw Gizmos
	for (auto e : m_entityManager.GetEntities())
	{
		if (e->HasComponent<CTransform>())
		{
			const auto& transform = e->GetComponent<CTransform>();

			if (e->HasComponent<CCircleShape>())
			{
				auto& circleShape = e->GetComponent<CCircleShape>();

				circleShape.shape.setPosition(transform.Static.Position.x, transform.Static.Position.y);

				m_game->window().draw(circleShape.shape);
			}

			if (e->HasComponent<CSteeringAI>())
			{
				auto& steeringAI = e->GetComponent<CSteeringAI>();

				auto otherE = m_entityManager.GetEntity(steeringAI.EntityID);
				if (e == otherE) continue;
				if (!e->HasComponent<CTransform>()) continue;
				const CTransform& targetTransform = otherE->GetComponent<CTransform>();

				DrawLine(transform.Static.Position, targetTransform.Static.Position, Color(1.0f, 0.0f, 0.0f));
				DrawLine(transform.Static.Position, transform.Static.Position + OrientationAsVector(transform.Static.Orientation).Normalize() * 50.0f, Color(1.0f, 1.0f, 0.0f));
				DrawLine(transform.Static.Position, transform.Static.Position + transform.Dynamic.Velocity.Normalize() * 50.0f, Color(0.5f, 0.5f, 0.5f));
			}
		}
	}

	m_game->window().display();
}

void SceneKinematic::STargetMovement()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CFollowMouse>()) continue;
		if (!e->HasComponent<CTransform>()) continue;
		
		CTransform& transform = e->GetComponent<CTransform>();
		transform.Static.Position = Vec2(sf::Mouse::getPosition(m_game->window()).x, sf::Mouse::getPosition(m_game->window()).y);
	}
}

void SceneKinematic::SSeek()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& target = e->GetComponent<CSteeringAI>();

		auto otherE = m_entityManager.GetEntity(target.EntityID);
		if (e == otherE) continue;
		if (!e->HasComponent<CTransform>()) continue;

		const CTransform& targetTransform = otherE->GetComponent<CTransform>();
		KinematicMovementsAlgorithms::SteeringOutput steering = KinematicMovementsAlgorithms::Seek(transform.Static, targetTransform.Static, target.MaxSpeed);
		KinematicMovementsAlgorithms::Update(transform.Static, steering);
	}
}

void SceneKinematic::SFlee()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& target = e->GetComponent<CSteeringAI>();

		for (auto otherE : m_entityManager.GetEntities())
		{
			if (e == otherE) continue;
			if (otherE->id() == target.EntityID)
			{
				if (otherE->HasComponent<CTransform>())
				{
					const CTransform& targetTransform = otherE->GetComponent<CTransform>();
					KinematicMovementsAlgorithms::SteeringOutput steering = KinematicMovementsAlgorithms::Flee(transform.Static, targetTransform.Static, target.MaxSpeed);
					KinematicMovementsAlgorithms::Update(transform.Static, steering);
				}
				break;
			}
		}
	}
}

void SceneKinematic::SArrive()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& target = e->GetComponent<CSteeringAI>();

		for (auto otherE : m_entityManager.GetEntities())
		{
			if (e == otherE) continue;
			if (otherE->id() == target.EntityID)
			{
				if (otherE->HasComponent<CTransform>())
				{
					const CTransform& targetTransform = otherE->GetComponent<CTransform>();
					KinematicMovementsAlgorithms::SteeringOutput steering = KinematicMovementsAlgorithms::Arrive(transform.Static, targetTransform.Static, target.MaxSpeed, 20.0f, 0.25f);
					KinematicMovementsAlgorithms::Update(transform.Static, steering);
				}
				break;
			}
		}
	}
}

void SceneKinematic::SWander()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& target = e->GetComponent<CSteeringAI>();

		KinematicMovementsAlgorithms::SteeringOutput steering = KinematicMovementsAlgorithms::Wander(transform.Static, target.MaxSpeed, 0.07f);
		KinematicMovementsAlgorithms::Update(transform.Static, steering);
	}
}

void SceneKinematic::OnEnd()
{
}

Vec2 SceneKinematic::GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	const Vec2& entitySize = entity->GetComponent<CAnimation>().animation.getSize();
	Vec2 entityPosition = Vec2(gridX * m_gridSize.x, gridY * m_gridSize.y) + entitySize / 2;
	entityPosition.y = m_game->window().getSize().y - entityPosition.y;
	return entityPosition;
}