#pragma once

#include "Components.h"
#include <string>
#include <tuple>

typedef std::tuple<
	CTransform,
	CLifeSpan,
	CInput,
	CBoundingBox,
	CAnimation,
	CGravity,
	CState,
	CTargetToSeek,
	CText
> ComponentTuple;

class Entity
{
public:
	void destroy();
	const size_t id() const;
	bool isActive() const;
	const std::string& tag() const;

	template <typename T>
	bool HasComponent() const
	{
		return GetComponent<T>().has;
	}

	template <typename T, typename... TArgs>
	T& AddComponent(TArgs&&... mArgs)
	{
		auto& component = GetComponent<T>();
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template<typename T>
	T& GetComponent()
	{
		return std::get<T>(m_components);
	}

	template<typename T>
	const T& GetComponent() const
	{
		return std::get<T>(m_components);
	}

	template<typename T>
	void RemoveComponent()
	{
		GetComponent<T>() = T();
	}

private:
	friend class EntityManager;

	bool m_active = true;
	std::string m_tag = "default";
	size_t m_id = 0;
	ComponentTuple m_components;

	// private constructor so we can never create
	// entities outside the EnitytManager which had friend access
	Entity(const size_t id, const std::string& tag);
};
