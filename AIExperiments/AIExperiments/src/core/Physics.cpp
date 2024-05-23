#include "Physics.h"
#include "Components.h"

Physics::Physics() {};

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return the overlap rectangle size of the bounding boxes of entity a and b
	const CBoundingBox& aBoundingBox = a->GetComponent<CBoundingBox>();
	const CBoundingBox& bBoundingBox = b->GetComponent<CBoundingBox>();
	const Vec2& aPos = a->GetComponent<CTransform>().pos;
	const Vec2& bPos = b->GetComponent<CTransform>().pos;
	Vec2 delta = Vec2(abs(aPos.x - bPos.x), abs(aPos.y - bPos.y));
	float overlapX = aBoundingBox.halfSize.x + bBoundingBox.halfSize.x - delta.x;
	float overlapY = aBoundingBox.halfSize.y + bBoundingBox.halfSize.y - delta.y;
	return Vec2(overlapX, overlapY);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// TODO: return previous overlap rectangle size of the bounding boxes of entity a and b
	//		 previous overlap uses the entity's previous position
	const CBoundingBox& aBoundingBox = a->GetComponent<CBoundingBox>();
	const CBoundingBox& bBoundingBox = b->GetComponent<CBoundingBox>();
	const Vec2& aPos = a->GetComponent<CTransform>().prevPos;
	const Vec2& bPos = b->GetComponent<CTransform>().prevPos;
	Vec2 delta = Vec2(abs(aPos.x - bPos.x), abs(aPos.y - bPos.y));
	float overlapX = aBoundingBox.halfSize.x + bBoundingBox.halfSize.x - delta.x;
	float overlapY = aBoundingBox.halfSize.y + bBoundingBox.halfSize.y - delta.y;
	return Vec2(overlapX, overlapY);
}
