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
			DEL_ENTITY,
			DEL_COMPONENT
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
		using System = System<GameObject>;
		using Command = std::variant<DeleteEntity, DeleteComponent>;

	public:
		VELOX_API ObjectSystem(EntityAdmin& entity_admin);

	public:
		VELOX_API Entity CreateObject() const;

		VELOX_API void DeleteObjectDelayed(const EntityID entity_id);
		VELOX_API void DeleteObjectInstant(const EntityID entity_id);

	public:
		template<IsComponentType C>
		void DeleteComponentDelayed(const EntityID entity_id);
		template<IsComponentType C>
		void DeleteComponentInstant(const EntityID entity_id);

	private:
		VELOX_API void Update();

	private:
		EntityAdmin*	m_entity_admin{nullptr};
		System			m_system;

		std::queue<std::pair<Command, CommandType>> m_deletion_queue;
	};

	template<IsComponentType C>
	void ObjectSystem::DeleteComponentDelayed(const EntityID entity_id)
	{
		m_deletion_queue.push(std::make_pair(DeleteComponent(entity_id, Component<C>::GetTypeID()), DEL_COMPONENT));
	}
	template<IsComponentType C>
	void ObjectSystem::DeleteComponentInstant(const EntityID entity_id)
	{
		m_entity_admin->RemoveComponent<C>(entity_id);
	}
}