#pragma once

#include <variant>
#include <array>
#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/System/Traits.h>
#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "Object.h"

namespace vlx
{
	class ObjectSystem final : public SystemAction
	{
	public:
		enum ExecutionStage : int8
		{
			S_Instant = -1,
			S_PreUpdate,
			S_Update,
			S_FixedUpdate,
			S_PostUpdate,
			S_Manual,
			S_Count
		};

	private:
		struct AddCompData
		{
			EntityID		entity_id;
			ComponentTypeID component_id;
		};

		struct AddCompsData
		{
			EntityID		entity_id;
			ComponentIDs	component_ids;
			ArchetypeID		archetype_id;
		};

		struct RmvCompData
		{
			EntityID		entity_id;
			ComponentTypeID component_id;
		};

		struct RmvCompsData
		{
			EntityID		entity_id;
			ComponentIDs	component_ids;
			ArchetypeID		archetype_id;
		};

		struct RmvEntityData
		{
			EntityID entity_id;
		};

	private:
		using Command = std::variant<AddCompData, AddCompsData, RmvCompData, RmvCompsData, RmvEntityData>;
		using CommandTable = std::array<std::queue<Command>, S_Count>;

	public:
		ObjectSystem(EntityAdmin& entity_admin, LayerType id);

	public:
		VELOX_API Entity CreateEntity() const;
		VELOX_API void RemoveEntity(EntityID entity_id, ExecutionStage stage = S_PostUpdate);

		VELOX_API void ExecuteManually();

	public:
		VELOX_API void Start() override;
		VELOX_API void PreUpdate() override;
		VELOX_API void Update() override;
		VELOX_API void FixedUpdate() override;
		VELOX_API void PostUpdate() override;

	public:
		template<IsComponent C>
		void AddComponent(EntityID entity_id, ExecutionStage stage = W_PostUpdate);
		template<class... Cs> requires IsComponents<Cs...>
		void AddComponents(EntityID entity_id, ExecutionStage stage = W_PostUpdate);
		template<class... Cs> requires IsComponents<Cs...>
		void AddComponents(EntityID entity_id, std::type_identity<std::tuple<Cs...>>, ExecutionStage stage = W_PostUpdate);

		template<IsComponent C>
		void RemoveComponent(EntityID entity_id, ExecutionStage stage = W_PostUpdate);
		template<class... Cs> requires IsComponents<Cs...>
		void RemoveComponents(EntityID entity_id, ExecutionStage stage = W_PostUpdate);
		template<class... Cs> requires IsComponents<Cs...>
		void RemoveComponents(EntityID entity_id, std::type_identity<std::tuple<Cs...>>, ExecutionStage stage = W_PostUpdate);

	private:
		void ExecuteCommands(ExecutionStage when);
		void VisitCommand(const Command& command);

	private:
		CommandTable m_command_table;
	};

	template<IsComponent C>
	inline void ObjectSystem::AddComponent(EntityID entity_id, ExecutionStage stage)
	{
		if (stage == S_Instant)
		{
			m_entity_admin->AddComponent<C>(entity_id);
			return;
		}

		m_command_table[stage].emplace(std::in_place_index<0>, entity_id, m_entity_admin->GetComponentID<C>());
	}
	template<class... Cs> requires IsComponents<Cs...>
	inline void ObjectSystem::AddComponents(EntityID entity_id, ExecutionStage stage)
	{
		if (stage == S_Instant)
		{
			m_entity_admin->AddComponents<Cs...>(entity_id);
			return;
		}

		constexpr auto component_ids = cu::Sort<ArrComponentIDs<Cs...>>({ m_entity_admin->GetComponentID<Cs>()... });
		constexpr auto archetype_id = cu::ContainerHash<ArrComponentIDs<Cs...>>()(component_ids);

		m_command_table[stage].emplace(std::in_place_index<1>, entity_id,
			ComponentIDs(component_ids.begin(), component_ids.end()), archetype_id);
	}
	template<class... Cs> requires IsComponents<Cs...>
	inline void ObjectSystem::AddComponents(EntityID entity_id, std::type_identity<std::tuple<Cs...>>, ExecutionStage stage)
	{
		AddComponents<Cs...>(entity_id, stage);
	}

	template<IsComponent C>
	inline void ObjectSystem::RemoveComponent(EntityID entity_id, ExecutionStage stage)
	{
		if (stage == S_Instant)
		{
			m_entity_admin->RemoveComponent<C>(entity_id);
			return;
		}

		m_command_table[stage].emplace(std::in_place_index<2>, entity_id, m_entity_admin->GetComponentID<C>());
	}
	template<class... Cs> requires IsComponents<Cs...>
	inline void ObjectSystem::RemoveComponents(EntityID entity_id, ExecutionStage stage)
	{
		if (stage == S_Instant)
		{
			m_entity_admin->RemoveComponents<Cs...>(entity_id);
			return;
		}

		constexpr auto component_ids = cu::Sort<ArrComponentIDs<Cs...>>({ m_entity_admin->GetComponentID<Cs>()... });
		constexpr auto archetype_id = cu::ContainerHash<ArrComponentIDs<Cs...>>()(component_ids);

		m_command_table[stage].emplace(std::in_place_index<3>, entity_id,
			ComponentIDs(component_ids.begin(), component_ids.end()), archetype_id);
	}
	template<class... Cs> requires IsComponents<Cs...>
	inline void ObjectSystem::RemoveComponents(EntityID entity_id, std::type_identity<std::tuple<Cs...>>, ExecutionStage stage)
	{
		RemoveComponents<Cs...>(entity_id, stage);
	}
}