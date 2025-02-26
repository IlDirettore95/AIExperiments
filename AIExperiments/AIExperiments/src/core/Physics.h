#pragma once

#include "../helpers/Vec2.h"
#include <memory>
#include "Entity.h"

class Physics
{
private:
	Physics();

public:
	static Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	static Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
};