#pragma once

#include <vector>
#include <map>
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVector;
typedef std::map<std::string, EntityVector> EntityMap;

class EntityManager
{
	EntityVector m_entities;
	EntityVector m_entitiesToAdd;
	EntityMap m_entityMap;
	size_t m_totalEntities = 0;

	void RemoveDeadEntities(EntityVector& v);

public:
	EntityManager();
	void Update();

	std::shared_ptr<Entity> AddEntity(const std::string& tag);

	const EntityVector& GetEntities() const; 
	const EntityVector& GetEntities(const std::string& tag);
	const EntityMap& GetEntityMap() const;
	const size_t GetTotalEntities() const;
};