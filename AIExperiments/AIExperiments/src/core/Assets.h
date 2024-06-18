#pragma once

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include <string>
#include "Animation.h"

class Assets
{
	std::map<std::string, sf::Texture> m_textures;
	std::map<std::string, Animation> m_animations;
	std::map<std::string, sf::Sound> m_sounds;
	std::map<std::string, sf::Font > m_fonts;

public:
	Assets();

	void LoadFromFile();

	void AddTexture(const std::string& name, const std::string& path);
	void AddAnimation(const std::string& name, Animation Animation);
	void AddSound(const std::string& name, const std::string& path);
	void AddFont(const std::string& name, const std::string& path);

	const sf::Texture& GetTexture(const std::string& name) const;
	const Animation& GetAnimation(const std::string& name) const;
	const sf::Sound& GetSound(const std::string& name) const;
	const sf::Font& GetFont(const std::string& name) const;
};
