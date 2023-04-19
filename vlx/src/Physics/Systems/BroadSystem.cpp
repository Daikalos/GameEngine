#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

template<class S>
BroadSystem::ShapeQTBehaviour<S>::ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system) :
	m_broad(broad_system),

	m_insertion(entity_admin, id),
	m_body_insertion(entity_admin, id),
	m_queries(entity_admin, id),
	m_body_queries(entity_admin, id)
{
	m_insertion.Each(
		[this](EntityID entity_id, S& s, Collider& c, Transform& t)
		{
			InsertShape(entity_id, &s, s.GetType(), &c, nullptr, &t);
		});

	m_body_insertion.Each(
		[this](EntityID entity_id, S& s, Collider& c, PhysicsBody& pb, Transform& t)
		{
			InsertShape(entity_id, &s, s.GetType(), &c, &pb, &t);
		});

	m_queries.Each(
		[this](EntityID entity_id, S& s, Collider& c, Transform& t)
		{
			QueryShape(entity_id, &s, s.GetType(), &c, nullptr, &t);
		});

	m_body_queries.Each(
		[this](EntityID entity_id, S& s, Collider& c, PhysicsBody& pb, Transform& t)
		{
			QueryShape(entity_id, &s, s.GetType(), &c, &pb, &t);
		});

	m_insertion.Exclude<PhysicsBody>();
	m_queries.Exclude<PhysicsBody>();

	m_insertion.SetPriority(2.0f);
	m_body_insertion.SetPriority(2.0f);
}

BroadSystem::ShapeQTBehaviour<Point>::ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system) :
	m_broad(broad_system),

	m_queries(entity_admin, id),
	m_body_queries(entity_admin, id)
{
	m_queries.Each(
		[this](EntityID entity_id, Point& p, Collider& c, Transform& t)
		{
			QueryPoint(entity_id, &p, &c, nullptr, &t);
		});

	m_body_queries.Each(
		[this](EntityID entity_id, Point& p, Collider& c, PhysicsBody& pb, Transform& t)
		{
			QueryPoint(entity_id, &p, &c, &pb, &t);
		});

	m_queries.Exclude<PhysicsBody>();
}

template<class S>
void BroadSystem::ShapeQTBehaviour<S>::InsertShape(EntityID entity_id, Shape* s, typename Shape::Type type, Collider* c, PhysicsBody* pb, Transform* t)
{
	if (c->dirty)
	{
		c->Erase();
		c->Insert(m_broad.m_quad_tree, s->GetAABB(), entity_id, type, s, c, pb, t);

		c->dirty = false;
	}
	else
	{
		c->Update(entity_id, type, s, c, pb, t); // attempt to update data if already inserted, needed for cases where pointers may be invalidated
	}
}

template<class S>
void BroadSystem::ShapeQTBehaviour<S>::QueryShape(EntityID entity_id, Shape* s, typename Shape::Type type, Collider* c, PhysicsBody* pb, Transform* t)
{
	auto collisions = m_broad.m_quad_tree.Query(s->GetAABB());

	for (const auto& collision : collisions)
	{
		if (s == collision.item.shape) // no collision against self
			continue;

		if ((c->layer & collision.item.collider->layer) == 0) // only matching layer
			continue;

		if (pb != nullptr && collision.item.body != nullptr)
		{
			const auto& lhs = pb;
			const auto& rhs = collision.item.body;

			bool lhs_active = (lhs->IsAwake() && lhs->IsEnabled());
			bool rhs_active = (rhs->IsAwake() && rhs->IsEnabled());

			if (!lhs_active && !rhs_active)
				continue;
		}

		m_broad.m_pairs.emplace_back(CollisionObject(entity_id, type, s, c, pb, t), collision.item);
		m_broad.m_indices.emplace_back(static_cast<uint32_t>(m_broad.m_pairs.size() - 1));
	}
}

void BroadSystem::ShapeQTBehaviour<Point>::QueryPoint(EntityID entity_id, Point* p, Collider* c, PhysicsBody* pb, Transform* t)
{
	auto collisions = m_broad.m_quad_tree.Query(p->GetPosition());

	for (const auto& collision : collisions)
	{
		// no need to check against self

		if ((c->layer & collision.item.collider->layer) == 0) // only matching layer
			continue;

		if (pb != nullptr && collision.item.body != nullptr)
		{
			const auto& lhs = pb;
			const auto& rhs = collision.item.body;

			bool lhs_active = (lhs->IsAwake() && lhs->IsEnabled());
			bool rhs_active = (rhs->IsAwake() && rhs->IsEnabled());

			if (!lhs_active && !rhs_active)
				continue;
		}

		m_broad.m_pairs.emplace_back(CollisionObject(entity_id, Shape::Point, p, c, pb, t), collision.item);
		m_broad.m_indices.emplace_back(static_cast<uint32_t>(m_broad.m_pairs.size() - 1));
	}
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
}

template class BroadSystem::ShapeQTBehaviour<Circle>;
template class BroadSystem::ShapeQTBehaviour<Box>;
