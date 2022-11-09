#pragma once

#include <variant>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>
#include <Velox/Utilities.hpp>

#include <Velox/Components/Object.h>

#include "ISystemObject.h"

namespace vlx
{
	class ObjectSystem : public ISystemObject
	{
	private:
		enum CommandType : std::uint8_t
		{
			ADD_COMPONENT,
			DEL_ENTITY,
			DEL_COMPONENT
		};

		struct AddComponent
		{
			EntityID		entity_id;
			ComponentTypeID component_id;
		};

		struct DeleteEntity
		{
			EntityID entity_id;
		};

		struct DeleteComponent
		{
			EntityID		entity_id;
			ComponentTypeID component_id;
		};

	private:
		using Command	= std::pair<std::variant<AddComponent, DeleteEntity, DeleteComponent>, CommandType>;

	public:
		VELOX_API ObjectSystem(EntityAdmin& entity_admin);

		VELOX_API Entity CreateObject() const;

		VELOX_API void DeleteObjectDelayed(const EntityID entity_id);
		VELOX_API void DeleteObjectInstant(const EntityID entity_id);

	public:
		VELOX_API void Update() override;

	private:
		VELOX_API void PostUpdate();

	public:
		template<IsComponent C>
		void DeleteComponentDelayed(const EntityID entity_id);
		template<IsComponent C>
		void DeleteComponentInstant(const EntityID entity_id);

		template<IsComponent C>
		void AddComponentDelayed(const EntityID entity_id);
		template<IsComponent... Cs>
		void AddComponentsDelayed(const EntityID entity_id);

		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		C* AddComponentInstant(const EntityID entity_id, Args&&... args);
		template<IsComponent... Cs>
		void AddComponentsInstant(const EntityID entity_id);

	private:
		EntityAdmin*							m_entity_admin	{nullptr};
		System<Object>							m_object_system;

		std::queue<Command>						m_commands;
	};

	template<IsComponent C>
	inline void ObjectSystem::DeleteComponentDelayed(const EntityID entity_id)
	{
		m_commands.emplace(DeleteComponent(entity_id, ComponentAlloc<C>::GetTypeID()), DEL_COMPONENT);
	}
	template<IsComponent C>
	inline void ObjectSystem::DeleteComponentInstant(const EntityID entity_id)
	{
		m_entity_admin->RemoveComponent<C>(entity_id);
	}

	template<IsComponent C>
	inline void ObjectSystem::AddComponentDelayed(const EntityID entity_id)
	{
		m_commands.emplace(AddComponent(entity_id, ComponentAlloc<C>::GetTypeID()), ADD_COMPONENT);
	}
	template<IsComponent... Cs>
	inline void ObjectSystem::AddComponentsDelayed(const EntityID entity_id)
	{
		(AddComponentDelayed<Cs>(entity_id), ...);
	}

	template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
	inline C* ObjectSystem::AddComponentInstant(const EntityID entity_id, Args&&... args)
	{
		return m_entity_admin->AddComponent(entity_id, std::forward<Args>(args)...);
	}
	template<IsComponent... Cs>
	inline void ObjectSystem::AddComponentsInstant(const EntityID entity_id)
	{
		m_entity_admin->AddComponents<Cs...>(entity_id);
	}
}