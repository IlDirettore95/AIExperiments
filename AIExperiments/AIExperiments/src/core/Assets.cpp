#include "Assets.h"
#include <fstream>
#include <iostream>
#include <filesystem>

Assets::Assets() {}

void Assets::LoadFromFile()
{
	std::fstream fin((std::filesystem::current_path() / "resources" / "assets.txt").string());
	std::string label;

	while (fin >> label)
	{
		if (label == "Texture")
		{
			std::string name;
			std::string path;

			fin >> name;
			fin >> path;

			AddTexture(name, path);
		}
		else if (label == "Animation")
		{
			std::string name;
			std::string textureName;
			int animationFrameCount = 0;
			int framesPerAnimationFrame = 0;

			fin >> name;
			fin >> textureName;
			fin >> animationFrameCount;
			fin >> framesPerAnimationFrame;

			AddAnimation(name, Animation(name, GetTexture(textureName), animationFrameCount, framesPerAnimationFrame));
		}
		else if (label == "Sound")
		{
			std::string name;
			std::string path;

			fin >> name;
			fin >> path;

			AddSound(name, path);
		}
		else if (label == "Font")
		{
			std::string name;
			std::string path;

			fin >> name;
			fin >> path;

			AddFont(name, path);
		}
	}
}

void Assets::AddTexture(const std::string& name, const std::string& path)
{
	sf::Texture texture;
	if (!texture.loadFromFile(path))
	{
		std::cout << "Texture: " << name << " was not found at " << path << std::endl;
		return;
	}

	m_textures[name] = texture;

#if GE_DEBUG
	std::cout << "Texture loaded! " << name << " " << path << std::endl;
#endif
}

void Assets::AddSound(const std::string& name, const std::string& path)
{
	sf::SoundBuffer soundBuffer;
	if (!soundBuffer.loadFromFile(path))
	{
		std::cout << "Sound: " << name << " was not found at " << path << std::endl;
		return;
	}

	sf::Sound sound;
	sound.setBuffer(soundBuffer);

	m_sounds[name] = sound;

#if GE_DEBUG
	std::cout << "Sound loaded! " << name << " " << path << std::endl;
#endif
}

void Assets::AddFont(const std::string& name, const std::string& path)
{
	sf::Font font;
	if (!font.loadFromFile(path))
	{
		std::cout << "Font: " << name << " was not found at " << path << std::endl;
		return;
	}

	m_fonts[name] = font;

#if GE_DEBUG
	std::cout << "Font loaded! " << name << " " << path << std::endl;
#endif
}

void Assets::AddAnimation(const std::string& name, Animation animation)
{
	m_animations[name] = animation;

#if GE_DEBUG
	std::cout << "Animation loaded! " << name << std::endl;
#endif
}

const sf::Texture& Assets::GetTexture(const std::string& name) const
{
	return m_textures.at(name);
}

const sf::Sound& Assets::GetSound(const std::string& name) const
{
	return m_sounds.at(name);
}

const sf::Font& Assets::GetFont(const std::string& name) const
{
	return m_fonts.at(name);
}

const Animation& Assets::GetAnimation(const std::string& name) const
{
	return m_animations.at(name);
}
