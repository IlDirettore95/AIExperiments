#include "GameEngine.h"
#include "../SceneMainMenu.h"
#include <filesystem>

void GameEngine::init()
{
	m_assets.LoadFromFile();
	m_window.create(sf::VideoMode(1280, 768), "AIExperiments!");
	m_window.setFramerateLimit(60);
	m_window.setKeyRepeatEnabled(false);

	std::string firstScenePath = (std::filesystem::current_path() / "resources" / "main_menu_scenedata.txt").string();
	changeScene("MAIN_MENU", std::make_shared<SceneMainMenu>(this, firstScenePath));
}

void GameEngine::update()
{
	while (m_running)
	{
		sUserInput();
		currentScene()->Update();
	}
}

void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
				case sf::Keyboard::X:
				{
					std::cout << "screenshot saved to " << "test.png" << std::endl;
					sf::Texture texture;
					texture.create(m_window.getSize().x, m_window.getSize().y);
					texture.update(m_window);
					if (texture.copyToImage().saveToFile("test.png"))
					{
						std::cout << "screenshot saved to " << "test.png" << std::endl;
					}
				}
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			// if the current scene does not have an action associated with this key, skip the event
			if (currentScene()->GetActionMap().find(event.key.code) == currentScene()->GetActionMap().end()) return;

			// determine start or end action by wether it was key pressed or released
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			// look up the action and send the action to the scene
			currentScene()->SDoAction(Action(currentScene()->GetActionMap().at(event.key.code), actionType));
		}
	}
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[m_currentScene];
}

GameEngine::GameEngine()
{
	init();
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (scene)
	{
		m_sceneMap[sceneName] = scene;
	}
	else
	{
		if (m_sceneMap.find(sceneName) == m_sceneMap.end())
		{
			std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
			return;
		}
	}

	if (endCurrentScene)
	{
		m_sceneMap.erase(m_sceneMap.find(m_currentScene));
	}

	m_currentScene = sceneName;
}

void GameEngine::quit()
{
	m_running = false;
}

void GameEngine::run()
{
	while (m_running)
	{
		update();
	}
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

const Assets& GameEngine::assets() const
{
	return m_assets;
}

bool GameEngine::isRunning()
{
	return m_running && m_window.isOpen();
}
