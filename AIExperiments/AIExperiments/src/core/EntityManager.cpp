#include "EntityManager.h"
#include <memory>

EntityManager::EntityManager() {}

void EntityManager::Update()
{
    for (auto e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->m_tag].push_back(e);
    }
    m_entitiesToAdd.clear();

    RemoveDeadEntities(m_entities);

    for (auto& [tag, entityVector] : m_entityMap)
    {
        RemoveDeadEntities(entityVector);
    }
}

std::shared_ptr<Entity> EntityManager::AddEntity(const std::string& tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_entitiesToAdd.push_back(entity);

    return entity;
}

void EntityManager::RemoveDeadEntities(EntityVector& v)
{
    std::erase_if(v, [](auto& entity) {return !entity->isActive(); });
}

std::shared_ptr<Entity> EntityManager::GetEntity(size_t id)
{
    for (auto e : m_entities)
    {
        if (e->id() == id)
        {
            return e;
        }
    }

    return nullptr;
}

const EntityVector& EntityManager::GetEntities() const
{
    return m_entities;
}

const EntityVector& EntityManager::GetEntities(const std::string& tag)
{
    return m_entityMap[tag];
}

const EntityMap& EntityManager::GetEntityMap() const
{
    return m_entityMap;
}

const size_t EntityManager::GetTotalEntities() const
{
    return m_totalEntities;
}
