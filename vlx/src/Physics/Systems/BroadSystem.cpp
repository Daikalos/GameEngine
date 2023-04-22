#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

template<class S>
BroadSystem::ShapeQTBehaviour<S>::ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system) :
	m_broad(broad_system),

	m_insert(entity_admin, id),
	m_body_insert(entity_admin, id),
	m_query(entity_admin, id),
	m_body_query(entity_admin, id)
{
	m_insert.Each(
		[this](EntityID entity_id, S& s, Collider& c)
		{
			InsertShape(entity_id, &s, s.GetType(), &c, nullptr);
		});

	m_body_insert.Each(
		[this](EntityID entity_id, S& s, Collider& c, PhysicsBody& pb)
		{
			InsertShape(entity_id, &s, s.GetType(), &c, &pb);
		});

	m_query.Each(
		[this](EntityID entity_id, S& s, Collider& c)
		{
			QueryShape(entity_id, &s, s.GetType(), &c, nullptr);
		});

	m_body_query.Each(
		[this](EntityID entity_id, S& s, Collider& c, PhysicsBody& pb)
		{
			QueryShape(entity_id, &s, s.GetType(), &c, &pb);
		});

	m_insert.Exclude<PhysicsBody>();
	m_query.Exclude<PhysicsBody>();

	m_insert.SetPriority(2.0f);
	m_body_insert.SetPriority(2.0f);
}

BroadSystem::ShapeQTBehaviour<Point>::ShapeQTBehaviour(EntityAdmin& entity_admin, const LayerType id, BroadSystem& broad_system) :
	m_broad(broad_system),

	m_query(entity_admin, id),
	m_body_query(entity_admin, id)
{
	m_query.Each(
		[this](EntityID entity_id, Point& p, Collider& c)
		{
			QueryPoint(entity_id, &p, &c, nullptr);
		});

	m_body_query.Each(
		[this](EntityID entity_id, Point& p, Collider& c, PhysicsBody& pb)
		{
			QueryPoint(entity_id, &p, &c, &pb);
		});

	m_query.Exclude<PhysicsBody>();
}

template<class S>
void BroadSystem::ShapeQTBehaviour<S>::InsertShape(EntityID entity_id, Shape* s, typename Shape::Type type, Collider* c, PhysicsBody* pb)
{
	if (c->dirty)
	{
		c->Erase();
		c->Insert(m_broad.m_quad_tree, s->GetAABB(), entity_id, type, s, c, pb);

		c->dirty = false;
	}
	else
	{
		c->Update(entity_id, type, s, c, pb); // attempt to update data if already inserted, needed for cases where pointers may be invalidated
	}
}

template<class S>
void BroadSystem::ShapeQTBehaviour<S>::QueryShape(EntityID entity_id, Shape* s, typename Shape::Type type, Collider* c, PhysicsBody* pb)
{
	auto collisions = m_broad.m_quad_tree.Query(s->GetAABB());

	for (const auto& collision : collisions)
	{
		if (s == collision.item.shape) // no collision against self
			continue;

		if (!c->enabled || !collision.item.collider->enabled)
			continue;

		if ((c->layer & collision.item.collider->layer) == 0) // only matching layer
			continue;

		if (pb != nullptr && collision.item.body != nullptr)
		{
			const auto& lhs = pb;
			const auto& rhs = collision.item.body;

			bool lhs_active = (lhs->IsAwake() && lhs->IsEnabled());
			bool rhs_active = (rhs->IsAwake() && rhs->IsEnabled());

			if (!lhs_active && !rhs_active) // dont bother colliding if both are either disabled or sleeping
				continue;
		}

		m_broad.m_pairs.emplace_back(CollisionObject(entity_id, type, s, c, pb), collision.item);
		m_broad.m_indices.emplace_back(static_cast<uint32_t>(m_broad.m_pairs.size() - 1));
	}
}

void BroadSystem::ShapeQTBehaviour<Point>::QueryPoint(EntityID entity_id, Point* p, Collider* c, PhysicsBody* pb)
{
	auto collisions = m_broad.m_quad_tree.Query(p->GetCenter());

	for (const auto& collision : collisions)
	{
		// no need to check against self

		if (!c->enabled || !collision.item.collider->enabled)
			continue;

		if ((c->layer & collision.item.collider->layer) == 0) // only matching layer
			continue;

		if (pb != nullptr && collision.item.body != nullptr)
		{
			const auto& lhs = pb;
			const auto& rhs = collision.item.body;

			bool lhs_active = (lhs->IsAwake() && lhs->IsEnabled());
			bool rhs_active = (rhs->IsAwake() && rhs->IsEnabled());

			if (!lhs_active && !rhs_active) // skip trying collision if both are either asleep or disabled
				continue;
		}

		m_broad.m_pairs.emplace_back(CollisionObject(entity_id, Shape::Point, p, c, pb), collision.item);
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
		m_quad_tree.Cleanup(); // have to cleanup in case of erase
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

auto BroadSystem::GetPairs() const noexcept -> std::span<const CollisionPair>
{
	return m_pairs;
}
auto BroadSystem::GetIndices() const noexcept -> std::span<const uint32>
{
	return m_indices;
}

auto BroadSystem::GetPairs() noexcept -> std::span<CollisionPair>
{
	return m_pairs;
}
auto BroadSystem::GetIndices() noexcept -> std::span<uint32>
{
	return m_indices;
}

void BroadSystem::CullDuplicates()
{
	std::ranges::sort(m_indices.begin(), m_indices.end(),
		[this](const uint32 l, const uint32 r)
		{
			const auto& x = m_pairs[l];
			const auto& y = m_pairs[r];

			return (x.first.entity_id < y.first.entity_id) || 
				(x.first.entity_id == y.first.entity_id && x.second.entity_id < y.second.entity_id);
		});

	const auto [first, last] = std::ranges::unique(m_indices.begin(), m_indices.end(),
		[this](const uint32 l, const uint32 r)
		{
			const auto& x = m_pairs[l];
			const auto& y = m_pairs[r];

			return x.first.entity_id == y.first.entity_id &&
				   y.second.entity_id == x.second.entity_id;
		});

	m_indices.erase(first, last);
}

template class BroadSystem::ShapeQTBehaviour<Circle>;
template class BroadSystem::ShapeQTBehaviour<Box>;
