#pragma once

#include <queue>
#include <variant>

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include <Velox/Graphics/Components/Relation.h>

namespace vlx
{
	class VELOX_API RelationSystem final : public SystemAction
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
		struct AttachData
		{
			EntityID parent_id;
			EntityID child_id;
		};

		struct DetachData
		{
			EntityID parent_id;
			EntityID child_id;
		};

	private:
		using Command = std::variant<AttachData, DetachData>;
		using CommandTable = std::array<std::queue<Command>, S_Count>;

	public:
		using SystemAction::SystemAction;

	public:
		void Attach(EntityID parent_id, EntityID child_id, ExecutionStage stage = S_PostUpdate);
		void Detach(EntityID parent_id, EntityID child_id, ExecutionStage stage = S_PostUpdate);

		void Attach(EntityID parent_id, Relation& parent, EntityID child_id, Relation& child);
		EntityID Detach(EntityID parent_id, Relation& parent, EntityID child_id, Relation& child);

		void ExecuteManually();

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		void AttachUnpack(EntityID parent_id, EntityID child_id);
		void DetachUnpack(EntityID parent_id, EntityID child_id);

		void ExecuteCommands(ExecutionStage stage);
		void VisitCommand(const Command& command);

	private:
		CommandTable m_commands_table;
	};
}