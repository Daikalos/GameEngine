#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

BroadSystem::BroadSystem(EntityAdmin& entity_admin, LayerType id) :
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

	m_add_coll_id = entity_admin.RegisterOnAddListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].collider = &c;
		});

	m_add_body_id = entity_admin.RegisterOnAddListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].body = &pb;
		});

	m_mov_coll_id = entity_admin.RegisterOnMoveListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			const auto it = m_entity_body_map.find(eid);
			if (it != m_entity_body_map.end())
				m_bodies[m_bodies_ptr[it->second].element].collider = &c;
		});

	m_mov_body_id = entity_admin.RegisterOnMoveListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			const auto it = m_entity_body_map.find(eid);
			if (it != m_entity_body_map.end())
				m_bodies[m_bodies_ptr[it->second].element].body = &pb;
		});

	m_rmv_coll_id = entity_admin.RegisterOnRemoveListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			const auto it = m_entity_body_map.find(eid);
			if (it != m_entity_body_map.end())
			{
				m_bodies[m_bodies_ptr[it->second].element].collider = nullptr;
				if (RemoveEmptyObject(it->second))
					m_entity_body_map.erase(it);
			}
		});

	m_rmv_body_id = entity_admin.RegisterOnRemoveListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			const auto it = m_entity_body_map.find(eid);
			if (it != m_entity_body_map.end())
			{
				m_bodies[m_bodies_ptr[it->second].element].body = nullptr;
				if (RemoveEmptyObject(it->second))
					m_entity_body_map.erase(it);
			}
		});

	m_cleanup.SetPriority(1.0f);
}

BroadSystem::~BroadSystem()
{
	m_entity_admin->DeregisterOnAddListener<Collider>(m_add_coll_id);
	m_entity_admin->DeregisterOnAddListener<PhysicsBody>(m_add_body_id);
	m_entity_admin->DeregisterOnMoveListener<Collider>(m_mov_coll_id);
	m_entity_admin->DeregisterOnMoveListener<PhysicsBody>(m_mov_body_id);
	m_entity_admin->DeregisterOnRemoveListener<Collider>(m_rmv_coll_id);
	m_entity_admin->DeregisterOnRemoveListener<PhysicsBody>(m_rmv_body_id);
}

void BroadSystem::Update()
{
	m_collisions.clear();

	m_entity_admin->RunSystems(m_layer);

	for (int i = m_first_body; i != -1; i = m_bodies_ptr[i].next)
	{
		const auto& ptr = m_bodies_ptr[i];
		const auto& lhs = m_bodies[ptr.element];

		if (lhs.shape == nullptr || lhs.collider == nullptr || !lhs.collider->GetEnabled())
			continue;

		std::vector<QuadTree::value_type> query;

		switch (lhs.type)
		{
		case Shape::Point:	query = m_quad_tree.Query(lhs.shape->GetCenter()); break;
		default:			query = m_quad_tree.Query(lhs.shape->GetAABB()); break;
		}

		for (QuadTree::value_type elt : query)
		{
			const auto& rhs = m_bodies[elt];
				
			if (ptr.element == elt) // skip same body
				continue;

			if (rhs.shape == nullptr || rhs.collider == nullptr || !rhs.collider->GetEnabled()) // safety checks
				continue;

			if (lhs.collider->layer.HasAny(rhs.collider->layer)) // only matching layer
				continue;

			if (lhs.body != nullptr && rhs.body != nullptr)
			{
				bool lhs_active = (lhs.body->IsAwake() && lhs.body->IsEnabled());
				bool rhs_active = (rhs.body->IsAwake() && rhs.body->IsEnabled());

				if (!lhs_active && !rhs_active) // skip attempting collision if both are either asleep or disabled
					continue;
			}

			m_collisions.emplace_back(ptr.element, elt);
		}
	}

	CullDuplicates();
}

auto BroadSystem::GetCollisions() const noexcept -> const CollisionList&
{
	return m_collisions;
}
auto BroadSystem::GetCollisions() noexcept -> CollisionList&
{
	return m_collisions;
}

const CollisionObject& BroadSystem::GetBody(uint32 i) const noexcept
{
	return m_bodies[i];
}

CollisionObject& BroadSystem::GetBody(uint32 i) noexcept
{
	return m_bodies[i];
}

void BroadSystem::CullDuplicates()
{
	std::ranges::sort(m_collisions.begin(), m_collisions.end(),
		[this](const CollisionPair& x, const CollisionPair& y)
		{
			return (x.first < y.first) || (x.first == y.first && x.second < y.second);
		});

	const auto [first, last] = std::ranges::unique(m_collisions.begin(), m_collisions.end(),
		[this](const CollisionPair& x, const CollisionPair& y)
		{
			return x.first == y.first && y.second == x.second;
		});

	m_collisions.erase(first, last);
}

int BroadSystem::TryAddNewObject(EntityID eid)
{
	const auto it = m_entity_body_map.find(eid);
	if (it == m_entity_body_map.end())
	{
		const auto i = m_bodies.emplace(eid);
		const auto j = m_bodies_ptr.emplace(i, m_first_body);

		if (m_first_body != -1)
		{
			assert(m_bodies_ptr.valid(m_first_body));
			m_bodies_ptr[m_first_body].prev = j;
		}

		m_entity_body_map.emplace(eid, j);

		m_first_body = j;

		return i;
	}

	return m_bodies_ptr[it->second].element;
}

bool BroadSystem::RemoveEmptyObject(uint32 index)
{
	const BodyPtr& ptr = m_bodies_ptr[index];
	const CollisionObject& object = m_bodies[ptr.element];

	if (object.body || object.collider || object.shape)
		return false;

	if (ptr.prev != -1) m_bodies_ptr[ptr.prev].next = ptr.next;
	if (ptr.next != -1) m_bodies_ptr[ptr.next].prev = ptr.prev;

	m_first_body = ptr.next;

	m_bodies.erase(ptr.element);
	m_bodies_ptr.erase(index);

	return true;
}
