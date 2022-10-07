#pragma once

#include <variant>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>
#include <Velox/Utilities.hpp>

#include <Velox/Components/GameObject.h>

namespace vlx
{
	class ObjectSystem
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
		using System	= System<GameObject>;
		using Command	= std::pair<std::variant<AddComponent, DeleteEntity, DeleteComponent>, CommandType>;

	public:
		VELOX_API ObjectSystem(EntityAdmin& entity_admin);

		VELOX_API Entity CreateObject() const;

		VELOX_API void DeleteObjectDelayed(const EntityID entity_id);
		VELOX_API void DeleteObjectInstant(const EntityID entity_id);

	public:
		template<IsComponentType C>
		void DeleteComponentDelayed(const EntityID entity_id);
		template<IsComponentType C>
		void DeleteComponentInstant(const EntityID entity_id);

		template<IsComponentType C>
		void AddComponentDelayed(const EntityID entity_id);
		template<IsComponentType... Cs>
		void AddComponentsDelayed(const EntityID entity_id);

		template<IsComponentType C, typename... Args> requires std::constructible_from<C, Args...>
		C* AddComponentInstant(const EntityID entity_id, Args&&... args);
		template<IsComponentType... Cs>
		void AddComponentsInstant(const EntityID entity_id);

	private:
		VELOX_API void Update();

	private:
		EntityAdmin*	m_entity_admin{nullptr};
		System			m_system;

		std::queue<Command>	m_command_queue;
	};

	template<IsComponentType C>
	void ObjectSystem::DeleteComponentDelayed(const EntityID entity_id)
	{
		m_command_queue.push(std::make_pair(DeleteComponent(entity_id, Component<C>::GetTypeID()), DEL_COMPONENT));
	}
	template<IsComponentType C>
	void ObjectSystem::DeleteComponentInstant(const EntityID entity_id)
	{
		m_entity_admin->RemoveComponent<C>(entity_id);
	}

	template<IsComponentType C>
	void ObjectSystem::AddComponentDelayed(const EntityID entity_id)
	{
		m_command_queue.push(std::make_pair(AddComponent(entity_id, Component<C>::GetTypeID()), ADD_COMPONENT));
	}
	template<IsComponentType... Cs>
	void ObjectSystem::AddComponentsDelayed(const EntityID entity_id)
	{
		(AddComponentDelayed<Cs>(entity_id),...);
	}

	template<IsComponentType C, typename... Args> requires std::constructible_from<C, Args...>
	C* ObjectSystem::AddComponentInstant(const EntityID entity_id, Args&&... args)
	{
		return m_entity_admin->AddComponent(entity_id, std::forward<Args>(args)...);
	}
	template<IsComponentType... Cs>
	void ObjectSystem::AddComponentsInstant(const EntityID entity_id)
	{
		(AddComponentInstant<Cs>(entity_id),...);
	}
}