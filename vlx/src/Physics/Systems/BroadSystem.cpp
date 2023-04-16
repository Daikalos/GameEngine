#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

template<class S>
BroadSystem::ShapeQTBehaviour<S>::ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system) :
	m_insertion(entity_admin, id),
	m_body_insertion(entity_admin, id),
	m_queries(entity_admin, id),
	m_body_queries(entity_admin, id)
{
	m_insertion.Each(
		[&broad_system](EntityID entity_id, S& s, Collision& c, LocalTransform& lt)
		{
			broad_system.InsertShape(entity_id, &s, s.GetType(), &c, nullptr, &lt);
		});

	m_body_insertion.Each(
		[&broad_system](EntityID entity_id, S& s, Collision& c, PhysicsBody& pb, LocalTransform& lt)
		{
			broad_system.InsertShape(entity_id, &s, s.GetType(), &c, &pb, &lt);
		});

	m_queries.Each(
		[&broad_system](EntityID entity_id, S& s, Collision& c, LocalTransform& lt)
		{
			broad_system.QueryShape(entity_id, &s, s.GetType(), &c, nullptr, &lt);
		});

	m_body_queries.Each(
		[&broad_system](EntityID entity_id, S& s, Collision& c, PhysicsBody& pb, LocalTransform& lt)
		{
			broad_system.QueryShape(entity_id, &s, s.GetType(), &c, &pb, &lt);
		});

	m_insertion.Exclude<PhysicsBody>();
	m_queries.Exclude<PhysicsBody>();

	m_insertion.SetPriority(2.0f);
	m_body_insertion.SetPriority(2.0f);
	m_queries.SetPriority(0.0f);
	m_body_queries.SetPriority(0.0f);
}

BroadSystem::ShapeQTBehaviour<Point>::ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system) :
	m_queries(entity_admin, id),
	m_body_queries(entity_admin, id)
{
	m_queries.Each(
		[&broad_system](EntityID entity_id, Point& p, Collision& c, LocalTransform& lt)
		{
			broad_system.QueryShape(entity_id, &p, p.GetType(), &c, nullptr, &lt);
		});

	m_body_queries.Each(
		[&broad_system](EntityID entity_id, Point& p, Collision& c, PhysicsBody& pb, LocalTransform& lt)
		{
			broad_system.QueryShape(entity_id, &p, p.GetType(), &c, &pb, &lt);
		});

	m_queries.Exclude<PhysicsBody>();

	m_queries.SetPriority(0.0f);
	m_body_queries.SetPriority(0.0f);
}

BroadSystem::BroadSystem(EntityAdmin& entity_admin, const LayerType id) : 
	m_entity_admin(&entity_admin), m_layer(id),

	m_quad_tree({ -4096, -4096, 4096 * 2, 4096 * 2 }), 

	m_circles(	entity_admin, id, *this),
	m_boxes(	entity_admin, id, *this),
	m_points(	entity_admin, id, *this),

	m_cleanup(	entity_admin, id)
{
	m_cleanup.OnStart += [this]()
	{
		m_quad_tree.Cleanup();
	};

	m_cleanup.SetPriority(1.0f);
}

void BroadSystem::Update()
{
	m_pairs.clear();
	m_indices.clear();

	m_entity_admin->RunSystems(m_layer);

	CullDuplicates();
}

auto BroadSystem::GetPairs() noexcept -> std::span<CollisionPair>
{
	return m_pairs;
}
auto BroadSystem::GetIndices() noexcept -> std::span<CollisionIndex>
{
	return m_indices;
}

auto BroadSystem::GetPairs() const noexcept -> std::span<const CollisionPair>
{
	return m_pairs;
}
auto BroadSystem::GetIndices() const noexcept -> std::span<const CollisionIndex>
{
	return m_indices;
}

void BroadSystem::InsertShape(EntityID entity_id, Shape* s, typename Shape::Type type, Collision* c, PhysicsBody* pb, LocalTransform* lt)
{
	if (c->dirty)
	{
		c->Erase();
		c->Insert(m_quad_tree, s->GetAABB(), entity_id, type, s, c, pb, lt);

		c->dirty = false;
	}
	else
	{
		c->Update(entity_id, type, s, c, pb, lt); // attempt to update data if already inserted, needed for cases where pointers may be invalidated
	}
}

void BroadSystem::QueryShape(EntityID entity_id, Shape* s, typename Shape::Type type, Collision* c, PhysicsBody* pb, LocalTransform* lt)
{
	auto collisions = m_quad_tree.Query(s->GetAABB());

	for (const auto& collision : collisions)
	{
		if (s == collision.item.shape) // no collision against self
			continue;

		if (!(c->layer & collision.item.collision->layer)) // only matching layer
			continue;

		m_pairs.emplace_back(CollisionObject(entity_id, type, s, c, pb, lt), collision.item);
		m_indices.emplace_back(static_cast<std::uint32_t>(m_pairs.size() - 1));
	}
}

void BroadSystem::CullDuplicates()
{
	std::ranges::sort(m_indices.begin(), m_indices.end(),
		[this](const auto lhs, const auto rhs)
		{
			const auto& clhs = m_pairs[lhs], &crhs = m_pairs[rhs];

			if (clhs.first.entity_id < crhs.first.entity_id)
				return true;

			if (clhs.first.entity_id == crhs.first.entity_id)
				return clhs.second.entity_id < crhs.second.entity_id;

			return false;
		});

	const auto [first, last] = std::ranges::unique(m_indices.begin(), m_indices.end(),
		[this](const auto lhs, const auto rhs)
		{
			const auto& clhs = m_pairs[lhs], &crhs = m_pairs[rhs];

			return	clhs.first.entity_id == crhs.second.entity_id && 
					clhs.second.entity_id == crhs.first.entity_id;
		});

	m_indices.erase(first, last);

	//std::uint32_t i = 0;
	//while (i < m_collision_indices.size())
	//{
	//	std::uint32_t i0 = m_collision_indices[i++];
	//	m_unique_collisions.emplace_back(i0);

	//	while (i < m_collision_indices.size())
	//	{
	//		std::uint32_t i1 = m_collision_indices[i];

	//		if (m_collision_pairs[i0].first.entity_id != m_collision_pairs[i1].second.entity_id || 
	//			m_collision_pairs[i0].second.entity_id != m_collision_pairs[i1].first.entity_id)
	//			break;

	//		++i;
	//	}
	//}
}

template class BroadSystem::ShapeQTBehaviour<Circle>;
template class BroadSystem::ShapeQTBehaviour<Box>;
