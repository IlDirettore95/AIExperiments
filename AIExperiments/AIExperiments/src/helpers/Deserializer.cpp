#include "Deserializer.h"
#include <string>


bool Deserializer::DeserializeTransform(std::fstream& fin, std::shared_ptr<Entity> entity)
{
	float posX = 0.0f;
	float posY = 0.0f;

	fin >> posX;
	fin >> posY;

	entity->AddComponent<CTransform>(Vec2(posX, posY));

	return true;
}

bool Deserializer::DeserializeText(std::fstream& fin, std::shared_ptr<Entity> entity, const sf::Font& textFont)
{
	std::string text;
	int fontSize = 0;
	std::string alignment;
	int r = 0;
	int g = 0;
	int b = 0;

	fin >> text;
	fin >> fontSize;
	fin >> alignment;
	fin >> r;
	fin >> g;
	fin >> b;

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

	std::replace(text.begin(), text.end(), '-', ' ');
	std::replace(text.begin(), text.end(), '|', '\n');
	entity->AddComponent<CText>(text, textFont, fontSize, sf::Color(r, g, b), alignmentType);

    return false;
}
