#pragma once

#include <queue>

#include <Velox/ECS.hpp>
#include <Velox/Config.hpp>

#include <Velox/Graphics/Components/Relation.h>

namespace vlx
{
	class VELOX_API RelationSystem final : public SystemAction
	{
	private:
		using EntityPair = std::pair<EntityID, EntityID>;

	public:
		using SystemAction::SystemAction;

	public:
		bool IsRequired() const noexcept override;

	public:
		void Attach(const EntityID parent_id, const EntityID child_id);
		void Detach(const EntityID parent_id, const EntityID child_id);

		void AttachInstant(const EntityID parent_id, const EntityID child_id);
		void DetachInstant(const EntityID parent_id, const EntityID child_id);

		void AttachDelay(const EntityID parent_id, const EntityID child_id);
		void DetachDelay(const EntityID parent_id, const EntityID child_id);

	public:
		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

	private:
		void AttachChild(const EntityID parent_id, const EntityID child_id);
		void DetachChild(const EntityID parent_id, const EntityID child_id);

		void AttachChild(const EntityID parent_id, Relation& parent, const EntityID child_id, Relation& child);
		EntityID DetachChild(const EntityID parent_id, Relation& parent, const EntityID child_id, Relation& child);

	private:
		std::queue<EntityPair>	m_attachments;
		std::queue<EntityPair>	m_detachments;
	};
}