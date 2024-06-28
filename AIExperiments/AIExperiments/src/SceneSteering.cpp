#include "SceneSteering.h"
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
#include "helpers/Deserializer.h"

SceneSteering::SceneSteering(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
{
	// SetRandomSeed	
	srand(time(NULL));

	// Register Action
	{
		RegisterAction(sf::Keyboard::Escape, "BACK_TO_MAIN_MENU");
		RegisterAction(sf::Keyboard::Space, "SWITCH_ALGORITHM");
		RegisterAction(sf::Keyboard::G, "TOGGLE_GIZMOS");
		RegisterAction(sf::Keyboard::Q, "ADD_WAYPOINT");
		RegisterAction(sf::Keyboard::W, "FOLLOW_PATH");
		RegisterAction(sf::Keyboard::E, "EREASE_PATH");
		RegisterAction(sf::Keyboard::R, "LOOP_PATH");
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
				auto entity = m_entityManager.AddEntity("AICharacter");
				Deserializer::DeserializeTransform(fin, entity);
				
				std::string animationName;
				fin >> animationName;

				entity->AddComponent<CAnimation>(m_game->assets().GetAnimation(animationName), true);

				CSteeringAI& aiSteering = entity->AddComponent<CSteeringAI>();
				aiSteering.EntityID = m_target->id();
				aiSteering.MaxSpeed = 2.0f;
				aiSteering.MaxAcceleration = 0.1f;
				aiSteering.MaxAngularSpeed = 0.2f;
				aiSteering.MaxAngularAcceleration = 0.05f;
				aiSteering.DistanceSlowRadius = 50.0f;
				aiSteering.DistanceTargetRadius = 20.0f;
				aiSteering.OrientationSlowRadius = 1.0f;
				aiSteering.OrientationTargetRadius = 0.001f;
				aiSteering.WanderOffset = 100.0f;
				aiSteering.WanderRadius = 15.0f;
				aiSteering.WanderRate = 0.5f;
				aiSteering.WanderOrientation = 0.0f;
				aiSteering.MaxPreditionTime = 60.0f;
				aiSteering.PathOffset = 50.0f;
				aiSteering.CurrentParam = 0.0f;
			}
			else if (label == "MovingTarget")
			{
				m_target = m_entityManager.AddEntity("MovingTarget");
				m_target->AddComponent<CTransform>();
				auto& followMouseComponent = m_target->AddComponent<CFollowMouse>();
				followMouseComponent.debugCircle.setRadius(5.0f);
				followMouseComponent.debugCircle.setPointCount(20);
			    followMouseComponent.debugCircle.setFillColor(sf::Color::White);
				followMouseComponent.debugCircle.setOutlineColor(sf::Color::White);
				followMouseComponent.debugCircle.setOutlineThickness(1.0f);
				followMouseComponent.debugCircle.setOrigin(followMouseComponent.debugCircle.getRadius(), followMouseComponent.debugCircle.getRadius());
			}
			else if (label == "TextAlgorithmType")
			{
				m_algorithmDescription = m_entityManager.AddEntity("Text");
				Deserializer::DeserializeTransform(fin, m_algorithmDescription);
				Deserializer::DeserializeText(fin, m_algorithmDescription, m_game->assets().GetFont("FontTech"));
				m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Seek");
			}
			else if (label == "Text")
			{
				auto entity = m_entityManager.AddEntity("Text");
				Deserializer::DeserializeTransform(fin, entity);
				Deserializer::DeserializeText(fin, entity, m_game->assets().GetFont("FontTech"));
			}
		}

	}
}

void SceneSteering::Update()
{
	m_entityManager.Update();

	STargetMovement();

	switch (m_algorithmType)
	{
	case ESteeringAlgorithmType::Seek:
	{
		SSeek();
		break;
	}
	case ESteeringAlgorithmType::Flee:
	{
		SFlee();
		break;
	}

	case ESteeringAlgorithmType::Arrive:
	{
		SArrive();
		break;
	}

	case ESteeringAlgorithmType::Wander:
	{
		SWander();
		break;
	}
	case ESteeringAlgorithmType::Pursue:
	{
		SPursue();
		break;
	}
	case ESteeringAlgorithmType::Evade:
	{
		SEvade();
		break;
	}
	case ESteeringAlgorithmType::PathFollowing:
	{
		SPathFollowing();
		break;
	}
	}

	SRenderer();

	m_currentFrame++;
}

void SceneSteering::SDoAction(const Action& action)
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
			ESteeringAlgorithmType oldAlgorithmType = m_algorithmType;
			// Exit actions
			switch (m_algorithmType)
			{
			case ESteeringAlgorithmType::Seek:
			{
				break;
			}

			case ESteeringAlgorithmType::Flee:
			{
				break;
			}

			case ESteeringAlgorithmType::Arrive:
			{
				break;
			}

			case ESteeringAlgorithmType::Wander:
			{
				break;
			}

			case ESteeringAlgorithmType::Pursue:
			{
				break;
			}

			case ESteeringAlgorithmType::Evade:
			{
				break;
			}

			case ESteeringAlgorithmType::PathFollowing:
			{
				for (auto& waypoint : m_entityManager.GetEntities("Waypoint"))
				{
					waypoint->destroy();
				}
				for (auto& e : m_entityManager.GetEntities())
				{
					if (!e->HasComponent<CSteeringAI>()) continue;

					auto& steeringAI = e->GetComponent<CSteeringAI>();
					steeringAI.CurrentParam = 0.0f;
				}
				m_currentPath.Clear();
				m_followPath = false;

				break;
			}
			}

			m_algorithmType = (ESteeringAlgorithmType)(((uint8_t)m_algorithmType + 1) % ((uint8_t)ESteeringAlgorithmType::Max));

			// Enter actions
			switch (m_algorithmType)
			{
			case ESteeringAlgorithmType::Seek:
			{
				m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Seek");
				break;
			}

			case ESteeringAlgorithmType::Flee:
			{
				m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Flee");
				break;
			}

			case ESteeringAlgorithmType::Arrive:
			{
				m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Arrive");
				break;
			}

			case ESteeringAlgorithmType::Wander:
			{
				m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Wander");
				break;
			}

			case ESteeringAlgorithmType::Pursue:
			{
				m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Pursue");
				break;
			}

			case ESteeringAlgorithmType::Evade:
			{
				m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: Evade");
				break;
			}
			case ESteeringAlgorithmType::PathFollowing:
			{
				m_algorithmDescription->GetComponent<CText>().text.setString("Algorithm: PathFollowing");
				break;

			}
			}
		}
		else if (action.Name() == "TOGGLE_GIZMOS")
		{
			m_drawGizmos = !m_drawGizmos;
		}
		else if (action.Name() == "ADD_WAYPOINT")
		{
			if (m_algorithmType == ESteeringAlgorithmType::PathFollowing)
			{
				auto& targetTransform = m_target->GetComponent<CTransform>();
				SpawnWaypoint(targetTransform.Static.Position);
				m_followPath = false;
			}
		}
		else if (action.Name() == "FOLLOW_PATH")
		{
			if (m_algorithmType == ESteeringAlgorithmType::PathFollowing)
			{
				m_followPath = true;
			}
		}
		else if (action.Name() == "EREASE_PATH")
		{
			if (m_algorithmType == ESteeringAlgorithmType::PathFollowing)
			{
				for (auto& waypoint : m_entityManager.GetEntities("Waypoint"))
				{
					waypoint->destroy();
				}
				for (auto& e : m_entityManager.GetEntities())
				{
					if (!e->HasComponent<CSteeringAI>()) continue;

					auto& steeringAI = e->GetComponent<CSteeringAI>();
					steeringAI.CurrentParam = 0.0f;
				}
				m_currentPath.Clear();
				m_followPath = false;
			}
		}
		else if (action.Name() == "LOOP_PATH")
		{
			if (m_algorithmType == ESteeringAlgorithmType::PathFollowing)
			{
				m_currentPath.Looped = !m_currentPath.Looped;
			}
		}
	}
	else if (action.Type() == "END")
	{

	}
}

void SceneSteering::SRenderer()
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

	// Draw Textures
	for (auto e : m_entityManager.GetEntities())
	{
		auto& transform = e->GetComponent<CTransform>();

		if (e->HasComponent<CAnimation>())
		{
			auto& Animation = e->GetComponent<CAnimation>().AnimationField;
			Animation.getSprite().setRotation(transform.Static.Orientation * (180.0f / M_PI));
			Animation.getSprite().setPosition(transform.Static.Position.x, transform.Static.Position.y);
			Animation.getSprite().setScale(transform.Scale.x, transform.Scale.y);

			m_game->window().draw(Animation.getSprite());
		}
	}

	// Draw Text
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CText>()) continue;
		if (!e->HasComponent<CTransform>()) continue;

		auto& transform = e->GetComponent<CTransform>();
		auto& text = e->GetComponent<CText>().text;
		text.setPosition(transform.Static.Position.x, transform.Static.Position.y);
		m_game->window().draw(text);
	}

	// Draw Waypoints
	for (auto e : m_entityManager.GetEntities("Waypoint"))
	{
		if (!e->HasComponent<CWaypoint>()) continue;
		if (!e->HasComponent<CTransform>()) continue;

		auto& waypointComponent = e->GetComponent<CWaypoint>();
		auto& transform = e->GetComponent<CTransform>();
		waypointComponent.debugCircle.setPosition(transform.Static.Position.x, transform.Static.Position.y);
		m_game->window().draw(waypointComponent.debugCircle);
	}

	// Draw Gizmos
	if (!m_drawGizmos)
	{
		m_game->window().setMouseCursorVisible(true);
	}
	else
	{
		for (auto e : m_entityManager.GetEntities())
		{
			if (e->HasComponent<CTransform>())
			{
				const auto& transform = e->GetComponent<CTransform>();

				if (e->HasComponent<CFollowMouse>())
				{
					auto& followMouseComponent = e->GetComponent<CFollowMouse>();

					followMouseComponent.debugCircle.setPosition(transform.Static.Position.x, transform.Static.Position.y);

					m_game->window().draw(followMouseComponent.debugCircle);
				}

				if (e->HasComponent<CSteeringAI>() && e->HasComponent<CTransform>())
				{
					auto& steeringAI = e->GetComponent<CSteeringAI>();
					auto& transformAI = e->GetComponent<CTransform>();

					if (steeringAI.EntityID == UINT64_MAX) break;
					auto otherE = m_entityManager.GetEntity(steeringAI.EntityID);
					if (e == otherE) continue;
					if (!e->HasComponent<CTransform>()) continue;
					const CTransform& targetTransform = otherE->GetComponent<CTransform>();

					switch (m_algorithmType)
					{
					case ESteeringAlgorithmType::Seek:
					{
						DrawLine(transform.Static.Position, targetTransform.Static.Position, Color(1.0f, 0.0f, 0.0f));
						break;
					}

					case ESteeringAlgorithmType::Flee:
					{
						DrawLine(transform.Static.Position, targetTransform.Static.Position, Color(1.0f, 0.0f, 0.0f));
						break;
					}

					case ESteeringAlgorithmType::Arrive:
					{
						DrawLine(transform.Static.Position, targetTransform.Static.Position, Color(1.0f, 0.0f, 0.0f));

						sf::CircleShape slowRadiusShape(steeringAI.DistanceSlowRadius, 40);
						slowRadiusShape.setFillColor(sf::Color::Transparent);
						slowRadiusShape.setOutlineColor(sf::Color::Green);
						slowRadiusShape.setOutlineThickness(1.0f);
						slowRadiusShape.setOrigin(steeringAI.DistanceSlowRadius, steeringAI.DistanceSlowRadius);
						slowRadiusShape.setPosition(targetTransform.Static.Position.x, targetTransform.Static.Position.y);
						m_game->window().draw(slowRadiusShape);
						break;
					}

					case ESteeringAlgorithmType::Wander:
					{
						Vec2 wanderCirclePosition = transformAI.Static.Position + OrientationAsVector(transformAI.Static.Orientation) * steeringAI.WanderOffset;

						sf::CircleShape wanderCircleShape(steeringAI.WanderRadius, 40);
						wanderCircleShape.setFillColor(sf::Color::Transparent);
						wanderCircleShape.setOutlineColor(sf::Color::Magenta);
						wanderCircleShape.setOutlineThickness(1.0f);
						wanderCircleShape.setOrigin(steeringAI.WanderRadius, steeringAI.WanderRadius);
						wanderCircleShape.setPosition(wanderCirclePosition.x, wanderCirclePosition.y);

						float wanderTargetOrientation = steeringAI.WanderOrientation + transformAI.Static.Orientation;
						Vec2 wanderTargetPosition = transformAI.Static.Position + OrientationAsVector(transformAI.Static.Orientation) * steeringAI.WanderOffset;
						wanderTargetPosition += OrientationAsVector(wanderTargetOrientation) * steeringAI.WanderRadius;

						sf::CircleShape wanderTargetShape(5.0f, 40);
						wanderTargetShape.setFillColor(sf::Color::Red);
						wanderTargetShape.setOutlineColor(sf::Color::Red);
						wanderTargetShape.setOutlineThickness(1.0f);
						wanderTargetShape.setOrigin(5.0f, 5.0f);
						wanderTargetShape.setPosition(wanderTargetPosition.x, wanderTargetPosition.y);

						DrawLine(transform.Static.Position, wanderTargetPosition, Color(1.0f, 0.0f, 1.0f));

						m_game->window().draw(wanderCircleShape);
						m_game->window().draw(wanderTargetShape);
						break;
					}

					case ESteeringAlgorithmType::Pursue:
					{
						Vec2 direction = targetTransform.Static.Position - transform.Static.Position;
						float distance = direction.Magnitude();
						float speed = transform.Dynamic.Velocity.Magnitude();
						float prediction = 0.0f;

						if (speed <= distance / steeringAI.MaxPreditionTime)
						{
							prediction = steeringAI.MaxPreditionTime;
						}
						else
						{
							prediction = distance / speed;
						}

						StaticData explicitTarget = targetTransform.Static;
						explicitTarget.Position += targetTransform.Dynamic.Velocity * prediction;
						explicitTarget.Orientation = 0.0f;

						DrawLine(transform.Static.Position, explicitTarget.Position, Color(1.0f, 0.0f, 0.0f));
						break;
					}

					case ESteeringAlgorithmType::Evade:
					{
						Vec2 direction = transform.Static.Position - targetTransform.Static.Position;
						float distance = direction.Magnitude();
						float speed = transform.Dynamic.Velocity.Magnitude();
						float prediction = 0.0f;

						if (speed <= distance / steeringAI.MaxPreditionTime)
						{
							prediction = steeringAI.MaxPreditionTime;
						}
						else
						{
							prediction = distance / speed;
						}

						StaticData explicitTarget = targetTransform.Static;
						explicitTarget.Position += targetTransform.Dynamic.Velocity * prediction;
						explicitTarget.Orientation = 0.0f;

						DrawLine(transform.Static.Position, explicitTarget.Position, Color(1.0f, 0.0f, 0.0f));
						break;
					}
					case ESteeringAlgorithmType::PathFollowing:
					{
						if (m_currentPath.Waypoints.size() <= 0) continue;
						DrawCircle(m_currentPath.GetPosition(steeringAI.CurrentParam), 2.0f, 10, Color(1.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f), 1.0f);

						float targetParam = steeringAI.CurrentParam + steeringAI.PathOffset;
						if (m_currentPath.Looped && targetParam > m_currentPath.Length) targetParam -= m_currentPath.Length;
						else if (!m_currentPath.Looped && targetParam > m_currentPath.Length) targetParam = m_currentPath.Length;

						DrawCircle(m_currentPath.GetPosition(targetParam), 7.0f, 10, Color(1.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f), 1.0f);
						break;
					}
					}

					DrawLine(transform.Static.Position, transform.Static.Position + OrientationAsVector(transform.Static.Orientation).Normalize() * 50.0f, Color(1.0f, 1.0f, 0.0f));
					DrawLine(transform.Static.Position, transform.Static.Position + transform.Dynamic.Velocity.Normalize() * 50.0f, Color(0.5f, 0.5f, 0.5f));
				}
			}
		}

		// Waypoints gizmos
		for (int i = 0; i < (int64_t)m_currentPath.Waypoints.size() - 1; i++)
		{
			const Vec2& start = m_currentPath.Waypoints.at(i);
			const Vec2& end = m_currentPath.Waypoints.at(i + 1);
			DrawLine(start, end, Color(0.8f, 0.2f, 0.8f));
		}
		if (m_currentPath.Waypoints.size() > 1 && m_currentPath.Looped)
		{
			DrawLine(m_currentPath.Waypoints.at(m_currentPath.Waypoints.size() - 1), m_currentPath.Waypoints.at(0), Color(0.8f, 0.2f, 0.8f));
		}
	}

	m_game->window().display();
}

void SceneSteering::STargetMovement()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CFollowMouse>()) continue;
		if (!e->HasComponent<CTransform>()) continue;
		
		CTransform& transform = e->GetComponent<CTransform>();
		Vec2 oldPosition = transform.Static.Position;
		transform.Static.Position = Vec2(sf::Mouse::getPosition(m_game->window()).x, sf::Mouse::getPosition(m_game->window()).y);
		transform.Dynamic.Velocity = transform.Static.Position - oldPosition;
	}
}

void SceneSteering::SSeek()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& steeringAI = e->GetComponent<CSteeringAI>();

		if (steeringAI.EntityID == UINT64_MAX) break;
		auto target = m_entityManager.GetEntity(steeringAI.EntityID);
		if (!target && target == e) continue;
		if (!target->HasComponent<CTransform>()) continue;
		const CTransform& targetTransform = target->GetComponent<CTransform>();

		SteeringMovementsAlgorithms::SteeringOutput steering;
		steering.Linear = SteeringMovementsAlgorithms::Seek(transform.Static, targetTransform.Static, steeringAI).Linear;
		steering.Angular = SteeringMovementsAlgorithms::LookWhereYouAreGoing(transform.Static, transform.Dynamic, steeringAI).Angular;
		SteeringMovementsAlgorithms::Update(transform.Static, transform.Dynamic, steering, steeringAI);
	}
}

void SceneSteering::SFlee()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& steeringAI = e->GetComponent<CSteeringAI>();

		if (steeringAI.EntityID == UINT64_MAX) break;
		auto target = m_entityManager.GetEntity(steeringAI.EntityID);
		if (!target && target == e) continue;
		if (!target->HasComponent<CTransform>()) continue;
		const CTransform& targetTransform = target->GetComponent<CTransform>();

		SteeringMovementsAlgorithms::SteeringOutput steering;
		steering.Linear = SteeringMovementsAlgorithms::Flee(transform.Static, targetTransform.Static, steeringAI).Linear;
		steering.Angular = SteeringMovementsAlgorithms::LookWhereYouAreGoing(transform.Static, transform.Dynamic, steeringAI).Angular;
		SteeringMovementsAlgorithms::Update(transform.Static, transform.Dynamic, steering, steeringAI);
	}
}

void SceneSteering::SArrive()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& steeringAI = e->GetComponent<CSteeringAI>();

		if (steeringAI.EntityID == UINT64_MAX) break;
		auto target = m_entityManager.GetEntity(steeringAI.EntityID);
		if (!target && target == e) continue;
		if (!target->HasComponent<CTransform>()) continue;
		const CTransform& targetTransform = target->GetComponent<CTransform>();

		SteeringMovementsAlgorithms::SteeringOutput steering;
		steering.Linear = SteeringMovementsAlgorithms::Arrive(transform.Static, transform.Dynamic, targetTransform.Static, steeringAI).Linear;
		steering.Angular = SteeringMovementsAlgorithms::LookWhereYouAreGoing(transform.Static, transform.Dynamic, steeringAI).Angular;
		SteeringMovementsAlgorithms::Update(transform.Static, transform.Dynamic, steering, steeringAI);
	}
}

void SceneSteering::SWander()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		CSteeringAI& aiSteering = e->GetComponent<CSteeringAI>();

		SteeringMovementsAlgorithms::SteeringOutput steering = SteeringMovementsAlgorithms::Wander(transform.Static, transform.Dynamic, aiSteering);
		SteeringMovementsAlgorithms::Update(transform.Static, transform.Dynamic, steering, aiSteering);
	}
}

void SceneSteering::SPursue()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& steeringAI = e->GetComponent<CSteeringAI>();

		if (steeringAI.EntityID == UINT64_MAX) break;
		auto target = m_entityManager.GetEntity(steeringAI.EntityID);
		if (!target && target == e) continue;
		if (!target->HasComponent<CTransform>()) continue;
		const CTransform& targetTransform = target->GetComponent<CTransform>();

		SteeringMovementsAlgorithms::SteeringOutput steering;
		steering.Linear = SteeringMovementsAlgorithms::Pursue(transform.Static, transform.Dynamic, targetTransform.Static, targetTransform.Dynamic, steeringAI).Linear;
		steering.Angular = SteeringMovementsAlgorithms::LookWhereYouAreGoing(transform.Static, transform.Dynamic, steeringAI).Angular;
		SteeringMovementsAlgorithms::Update(transform.Static, transform.Dynamic, steering, steeringAI);
	}
}

void SceneSteering::SEvade()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& steeringAI = e->GetComponent<CSteeringAI>();

		if (steeringAI.EntityID == UINT64_MAX) break;
		auto target = m_entityManager.GetEntity(steeringAI.EntityID);
		if (!target && target == e) continue;
		if (!target->HasComponent<CTransform>()) continue;
		const CTransform& targetTransform = target->GetComponent<CTransform>();

		SteeringMovementsAlgorithms::SteeringOutput steering;
		steering.Linear = SteeringMovementsAlgorithms::Evade(transform.Static, transform.Dynamic, targetTransform.Static, targetTransform.Dynamic, steeringAI).Linear;
		steering.Angular = SteeringMovementsAlgorithms::LookWhereYouAreGoing(transform.Static, transform.Dynamic, steeringAI).Angular;
		SteeringMovementsAlgorithms::Update(transform.Static, transform.Dynamic, steering, steeringAI);
	}
}

void SceneSteering::SPathFollowing()
{
	if (!m_followPath) return;

	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		CSteeringAI& steeringAI = e->GetComponent<CSteeringAI>();

		SteeringMovementsAlgorithms::SteeringOutput steering;
		steering.Linear = SteeringMovementsAlgorithms::PathFollowing(transform.Static, transform.Dynamic, steeringAI, m_currentPath).Linear;
		steering.Angular = SteeringMovementsAlgorithms::LookWhereYouAreGoing(transform.Static, transform.Dynamic, steeringAI).Angular;
		SteeringMovementsAlgorithms::Update(transform.Static, transform.Dynamic, steering, steeringAI);
	}
}


void SceneSteering::OnEnd()
{
}

void SceneSteering::SpawnWaypoint(const Vec2& position)
{
	auto waypoint = m_entityManager.AddEntity("Waypoint");
	waypoint->AddComponent<CTransform>(position);
	auto& waypointComponent = waypoint->AddComponent<CWaypoint>();
	waypointComponent.debugCircle.setRadius(10.0f);
	waypointComponent.debugCircle.setPointCount(20);
	waypointComponent.debugCircle.setFillColor(sf::Color::Transparent);
	waypointComponent.debugCircle.setOutlineColor(sf::Color::Magenta);
	waypointComponent.debugCircle.setOutlineThickness(1.0f);
	waypointComponent.debugCircle.setOrigin(waypointComponent.debugCircle.getRadius(), waypointComponent.debugCircle.getRadius());

	m_currentPath.AddWayPoint(position);
}

Vec2 SceneSteering::GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	const Vec2& entitySize = entity->GetComponent<CAnimation>().AnimationField.getSize();
	Vec2 entityPosition = Vec2(gridX * m_gridSize.x, gridY * m_gridSize.y) + entitySize / 2;
	entityPosition.y = m_game->window().getSize().y - entityPosition.y;
	return entityPosition;
}