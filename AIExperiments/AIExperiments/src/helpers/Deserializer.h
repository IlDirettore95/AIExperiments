#pragma once

#include <memory>
#include <fstream> 
#include "..\core\Entity.h"

namespace Deserializer
{
	bool DeserializeTransform(std::fstream& fin, std::shared_ptr<Entity> textEntity);
	bool DeserializeText(std::fstream& fin, std::shared_ptr<Entity> textEntity, const sf::Font& textFont);
}
