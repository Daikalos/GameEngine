#include <Velox/Graphics/Systems/RelationSystem.h>

using namespace vlx;

void RelationSystem::Attach(EntityID parent_id, EntityID child_id, ExecutionStage stage)
{
	if (stage == S_Instant)
	{
		AttachUnpack(parent_id, child_id);
		return;
	}

	m_commands_table[stage].emplace(std::in_place_index<0>, parent_id, child_id);
}
void RelationSystem::Detach(EntityID parent_id, EntityID child_id, ExecutionStage stage)
{
	if (stage == S_Instant)
	{
		DetachUnpack(parent_id, child_id);
		return;
	}

	m_commands_table[stage].emplace(std::in_place_index<1>, parent_id, child_id);
}

void RelationSystem::Attach(EntityID parent_id, Relation& parent, EntityID child_id, Relation& child)
{
	if (child.HasParent() && child.GetParent().entity_id == parent_id) // child is already correctly parented
		return;

	if (parent_id == child_id) // cant attach to itself
		return;

#if _DEBUG
	if (parent.IsDescendant(child_id))
		throw std::runtime_error("The new parent cannot be a descendant of the child");
#endif

	if (parent.HasParent() && parent.GetParent().entity_id == child_id) // special case
	{
		Detach(parent.m_parent.entity_id, *parent.m_parent.ptr, parent_id, parent);
		return;
	}

	if (child.HasParent()) // if child already has an attached parent we need to detach it
		Detach(child.m_parent.entity_id, *child.m_parent.ptr, child_id, child);

	child.m_parent.ptr = m_entity_admin->GetComponentRef<Relation>(parent_id, &parent);
	child.m_parent.entity_id = parent_id;

	parent.m_children.emplace_back(m_entity_admin->GetComponentRef<Relation>(child_id, &child), child_id);
}

EntityID RelationSystem::Detach(EntityID parent_id, Relation& parent, EntityID child_id, Relation& child)
{
	auto found = std::find_if(parent.m_children.begin(), parent.m_children.end(),
		[&child_id](const Relation::Ref& ptr)
		{
			return ptr.entity_id == child_id;
		});

	if (found == parent.m_children.end())
		return NULL_ENTITY;

	child.m_parent.ptr.Reset();
	child.m_parent.entity_id = NULL_ENTITY;

	*found = parent.m_children.back();
	parent.m_children.pop_back();

	return child_id;
}

void RelationSystem::ExecuteManually()
{
	ExecuteCommands(S_Manual);
}

void RelationSystem::PreUpdate()
{
	ExecuteCommands(S_PreUpdate);
}

void RelationSystem::Update()
{
	ExecuteCommands(S_Update);
}

void RelationSystem::FixedUpdate()
{
	ExecuteCommands(S_FixedUpdate);
}

void RelationSystem::PostUpdate()
{
	ExecuteCommands(S_PostUpdate);
}

void RelationSystem::AttachUnpack(EntityID parent_id, EntityID child_id)
{
	auto parent = m_entity_admin->TryGetComponent<Relation>(parent_id);
	if (parent.has_value())
	{
		auto child = m_entity_admin->TryGetComponent<Relation>(child_id);
		if (child.has_value())
		{
			Attach(parent_id, *parent.value(), child_id, *child.value());
		}
	}
}

void RelationSystem::DetachUnpack(EntityID parent_id, EntityID child_id)
{
	auto parent = m_entity_admin->TryGetComponent<Relation>(parent_id);
	if (parent.has_value())
	{
		auto child = m_entity_admin->TryGetComponent<Relation>(child_id);
		if (child.has_value())
		{
			Detach(parent_id, *parent.value(), child_id, *child.value());
		}
	}
}

void RelationSystem::ExecuteCommands(ExecutionStage stage)
{
	auto& commands = m_commands_table[stage];
	while (!commands.empty())
	{
		VisitCommand(commands.front());
		commands.pop();
	}
}

void RelationSystem::VisitCommand(const Command& command)
{
	std::visit(traits::Overload
		{
			[this](const AttachData& data)
			{
				AttachUnpack(data.parent_id, data.child_id);
			},
			[this](const DetachData& data)
			{
				DetachUnpack(data.parent_id, data.child_id);
			}
		}, command);
}
