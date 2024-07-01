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
#include "helpers/Deserializer.h"
#include <imgui-SFML.h>
#include <imgui.h>

SceneKinematic::SceneKinematic(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
{
	// SetRandomSeed	
	srand(time(NULL));

	// Register Action
	{
		RegisterAction(sf::Keyboard::Escape, "BACK_TO_MAIN_MENU");
		RegisterAction(sf::Keyboard::Space, "SWITCH_ALGORITHM");
		RegisterAction(sf::Keyboard::G, "TOGGLE_GIZMOS");
	}

	// Init
	ChangeBehaviour(EKinematicAlgorithmType::Seek);

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
				m_ai = m_entityManager.AddEntity("AICharacter");
				Deserializer::DeserializeTransform(fin, m_ai);

				std::string animationName;
				fin >> animationName;;

				m_ai->AddComponent<CAnimation>(m_game->assets().GetAnimation(animationName), true);
				
				CSteeringAI& aiSteering = m_ai->AddComponent<CSteeringAI>();
				aiSteering.EntityID = m_target->id();
				aiSteering.MaxSpeed = 2.0f;
				aiSteering.MaxAcceleration = 0.0f;
				aiSteering.MaxAngularSpeed = 0.0f;
				aiSteering.MaxAngularAcceleration = 0.0f;
				aiSteering.DistanceSlowRadius = 20.0f;
				aiSteering.DistanceTargetRadius = 0.0f;
				aiSteering.OrientationSlowRadius = 0.0f;
				aiSteering.OrientationTargetRadius = 0.0f;
				aiSteering.WanderOffset = 0.0f;
				aiSteering.WanderRadius = 0.0f;
				aiSteering.WanderRate = 0.0f;
				aiSteering.WanderOrientation = 0.0f;
				aiSteering.MaxPreditionTime = 0.0f;
				aiSteering.PathOffset = 0.0f;
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
				followMouseComponent.debugCircle.setOrigin(5.0f, 5.0f);
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

void SceneKinematic::Update()
{
	m_entityManager.Update();

	STargetMovement();

	switch (m_algorithmType)
	{
	case EKinematicAlgorithmType::Seek:
	{
		SSeek();
		break;
	}
	case EKinematicAlgorithmType::Flee:
	{
		SFlee();
		break;
	}
	case EKinematicAlgorithmType::Arrive:
	{
		SArrive();
		break;
	}
	case EKinematicAlgorithmType::Wander:
	{
		SWander();
		break;
	}
	}

	SGui();
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
			ChangeBehaviour((EKinematicAlgorithmType)(((uint8_t)m_algorithmType + 1) % ((uint8_t)EKinematicAlgorithmType::Max)));
		}
		else if (action.Name() == "TOGGLE_GIZMOS")
		{
			m_drawGizmos = !m_drawGizmos;
		}
	}
	else if (action.Type() == "END")
	{

	}
}

void SceneKinematic::SGui()
{
	// update imgui for this frame with the time the last frame took
	ImGui::SFML::Update(m_game->window(), m_deltaClock.restart());

	// DEMO
	//ImGui::ShowDemoWindow();

	// GUI
	ImGui::SetNextWindowSize(ImVec2(340.0f, 720.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	ImGui::Begin("Steering Behaviours", nullptr, windowFlags);

	ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("Tab", tabFlags))
	{
		if (ImGui::BeginTabItem("Configs"))
		{
			const char* behaviours[] = { "Seek", "Flee", "Arrive", "Wander"};
			static int currentBehaviour = (uint8_t)m_algorithmType;
			currentBehaviour = (uint8_t)m_algorithmType;
			ImGui::ListBox("", &currentBehaviour, behaviours, IM_ARRAYSIZE(behaviours), 4);

			if (currentBehaviour != (uint8_t)m_algorithmType)
			{
				ChangeBehaviour((EKinematicAlgorithmType)currentBehaviour);
			}

			CSteeringAI& aiSteering = m_ai->GetComponent<CSteeringAI>();
			ImGui::PushItemWidth(-200.0f);
			ImGui::InputFloat("Max Speed", &aiSteering.MaxSpeed, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Max Acceleration", &aiSteering.MaxAcceleration, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Max Angular Speed", &aiSteering.MaxAngularSpeed, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Max Angular Acceleration", &aiSteering.MaxAngularAcceleration, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Distance Slow Radius", &aiSteering.DistanceSlowRadius, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Distance Target Radius", &aiSteering.DistanceTargetRadius, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Orientation Slow Radius", &aiSteering.OrientationSlowRadius, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Orientation Target radius", &aiSteering.OrientationTargetRadius, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Wander Offset", &aiSteering.WanderOffset, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Wander Radius", &aiSteering.WanderRadius, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Wander Rate", &aiSteering.WanderRate, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Max Prediction Time", &aiSteering.MaxPreditionTime, 0.01f, 1.0f, "%.3f");
			ImGui::InputFloat("Path Offset", &aiSteering.PathOffset, 0.01f, 1.0f, "%.3f");

			ImGui::SeparatorText("Description");

			ImGui::TextWrapped(
				"%s"
				"\n\n%s", m_algorithmTypeDescription.c_str(), m_algorithmDescription.c_str());

			ImGui::SeparatorText("Other controls");
			ImGui::TextWrapped(
				"\nPress G to toggle gizmos"
				"\nPress Space to change algorithm"
				"\nPress Esc to go back to MainMenu");

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

	// BEHAVIOUR GUI
	ImGui::SetNextWindowSize(ImVec2(160.0f, 20.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowPos(ImVec2(1280.0f, 0.0f), ImGuiCond_Appearing, ImVec2(1.0f, 0.0f));

	ImGuiWindowFlags textWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground;
	ImGui::Begin("Behaviour Text", nullptr, textWindowFlags);

	ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), m_algorithmTypeDescription.c_str());

	ImGui::End();
}

void SceneKinematic::SRenderer()
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
			auto& Animation = e->GetComponent<CAnimation>().AnimationField;
			Animation.getSprite().setRotation(transform.Static.Orientation * (180.0f / M_PI));
			Animation.getSprite().setPosition(transform.Static.Position.x, transform.Static.Position.y);
			Animation.getSprite().setScale(transform.Scale.x, transform.Scale.y);
			m_game->window().draw(Animation.getSprite());
		}
	}

	// Draw Gizmos
	if (!m_drawGizmos)
	{
		m_game->window().setMouseCursorVisible(true);
	}
	else
	{
		m_game->window().setMouseCursorVisible(false);

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

				if (e->HasComponent<CSteeringAI>())
				{
					auto& steeringAI = e->GetComponent<CSteeringAI>();

					auto otherE = m_entityManager.GetEntity(steeringAI.EntityID);
					if (e == otherE) continue;
					if (!e->HasComponent<CTransform>()) continue;
					const CTransform& targetTransform = otherE->GetComponent<CTransform>();

					switch (m_algorithmType)
					{
						case EKinematicAlgorithmType::Seek:
						{
							DrawLine(transform.Static.Position, targetTransform.Static.Position, Color(1.0f, 0.0f, 0.0f));
							break;
						}

						case EKinematicAlgorithmType::Flee:
						{
							DrawLine(transform.Static.Position, targetTransform.Static.Position, Color(1.0f, 0.0f, 0.0f));
							break;
						}

						case EKinematicAlgorithmType::Arrive:
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

						case EKinematicAlgorithmType::Wander:
						{
							break;
						}
					}
				
					DrawLine(transform.Static.Position, transform.Static.Position + OrientationAsVector(transform.Static.Orientation).Normalize() * 50.0f, Color(1.0f, 1.0f, 0.0f));
					DrawLine(transform.Static.Position, transform.Static.Position + transform.Dynamic.Velocity.Normalize() * 50.0f, Color(0.5f, 0.5f, 0.5f));
				}
			}
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

	ImGui::SFML::Render(m_game->window()); // draw the ui last so it's on top


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
		const CSteeringAI& aiSteering = e->GetComponent<CSteeringAI>();

		auto target = m_entityManager.GetEntity(aiSteering.EntityID);
		if (!target && target == e) continue;
		if (!target->HasComponent<CTransform>()) continue;
		const CTransform& targetTransform = target->GetComponent<CTransform>();

		KinematicMovementsAlgorithms::SteeringOutput steering = KinematicMovementsAlgorithms::Seek(transform.Static, targetTransform.Static, aiSteering.MaxSpeed);
		transform.Static.Orientation = KinematicMovementsAlgorithms::NewOrientation(transform.Static.Orientation, steering.Velocity);
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
		const CSteeringAI& aiSteering = e->GetComponent<CSteeringAI>();

		auto target = m_entityManager.GetEntity(aiSteering.EntityID);
		if (!target && target == e) continue;
		if (!target->HasComponent<CTransform>()) continue;
		const CTransform& targetTransform = target->GetComponent<CTransform>();

		KinematicMovementsAlgorithms::SteeringOutput steering = KinematicMovementsAlgorithms::Flee(transform.Static, targetTransform.Static, aiSteering.MaxSpeed);
		transform.Static.Orientation = KinematicMovementsAlgorithms::NewOrientation(transform.Static.Orientation, steering.Velocity);
		KinematicMovementsAlgorithms::Update(transform.Static, steering);
	}
}

void SceneKinematic::SArrive()
{
	for (auto e : m_entityManager.GetEntities())
	{
		if (!e->HasComponent<CTransform>()) continue;
		if (!e->HasComponent<CSteeringAI>()) continue;

		CTransform& transform = e->GetComponent<CTransform>();
		const CSteeringAI& aiSteering = e->GetComponent<CSteeringAI>();

		auto target = m_entityManager.GetEntity(aiSteering.EntityID);
		if (!target && target == e) continue;
		if (!target->HasComponent<CTransform>()) continue;
		const CTransform& targetTransform = target->GetComponent<CTransform>();

		KinematicMovementsAlgorithms::SteeringOutput steering = KinematicMovementsAlgorithms::Arrive(transform.Static, targetTransform.Static, aiSteering.MaxSpeed, aiSteering.DistanceSlowRadius, 0.25f);
		transform.Static.Orientation = KinematicMovementsAlgorithms::NewOrientation(transform.Static.Orientation, steering.Velocity);
		KinematicMovementsAlgorithms::Update(transform.Static, steering);
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

		if (transform.Static.Position.x > m_game->window().getSize().x)
		{
			transform.Static.Position.x = m_game->window().getSize().x - transform.Static.Position.x;
		}
		else if (transform.Static.Position.x < 0.0f)
		{
			transform.Static.Position.x = m_game->window().getSize().x + transform.Static.Position.x;
		}

		if (transform.Static.Position.y > m_game->window().getSize().y)
		{
			transform.Static.Position.y = m_game->window().getSize().y - transform.Static.Position.y;
		}
		else if (transform.Static.Position.y < 0.0f)
		{
			transform.Static.Position.y = m_game->window().getSize().y + transform.Static.Position.y;
		}
	}
}

void SceneKinematic::OnEnd()
{
}

void SceneKinematic::ChangeBehaviour(EKinematicAlgorithmType newAlgorithm)
{
	// Enter actions
	switch (newAlgorithm)
	{
	case EKinematicAlgorithmType::Seek:
	{
		m_algorithmTypeDescription = "Algorithm: Seek";
		m_algorithmDescription = "Move towards target (mouse)";
		break;
	}

	case EKinematicAlgorithmType::Flee:
	{
		m_algorithmTypeDescription = "Algorithm: Flee";
		m_algorithmDescription = "Flee from target (mouse)";
		break;
	}

	case EKinematicAlgorithmType::Arrive:
	{
		m_algorithmTypeDescription = "Algorithm: Arrive";
		m_algorithmDescription = "Move towards target (mouse) and\nstop when it it near enough";
		break;
	}

	case EKinematicAlgorithmType::Wander:
	{
		m_algorithmTypeDescription = "Algorithm: Wander";
		m_algorithmDescription = "Wander randomly";
		break;
	}
	}

	m_algorithmType = newAlgorithm;
}

Vec2 SceneKinematic::GridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	const Vec2& entitySize = entity->GetComponent<CAnimation>().AnimationField.getSize();
	Vec2 entityPosition = Vec2(gridX * m_gridSize.x, gridY * m_gridSize.y) + entitySize / 2;
	entityPosition.y = m_game->window().getSize().y - entityPosition.y;
	return entityPosition;
}