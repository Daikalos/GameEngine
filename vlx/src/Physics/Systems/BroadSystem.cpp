#include <Velox/Physics/Systems/BroadSystem.h>

using namespace vlx;

BroadSystem::BroadSystem(EntityAdmin& entity_admin, LayerType id) :
	m_entity_admin(&entity_admin), m_layer(id),

	m_quad_tree({ -4096, -4096, 4096 * 2, 4096 * 2 }), // hard set size for now

	m_circles(	entity_admin, id, *this),
	m_boxes(	entity_admin, id, *this),
	m_points(	entity_admin, id, *this),
	m_polygons( entity_admin, id, *this)
{
	RegisterEvents();
}

BroadSystem::~BroadSystem()
{
	DeregisterEvents();
}

void BroadSystem::Update()
{
	m_collisions.clear();

	m_entity_admin->RunSystems(m_layer); // insert all shapes

	m_quad_tree.Cleanup(); // have to cleanup in case of erase

	GatherPossibleCollisions();

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

void BroadSystem::GatherPossibleCollisions()
{
	for (int i = m_first_body; i != -1; i = m_bodies_ptr[i].next)
	{
		const auto& ptr = m_bodies_ptr[i];
		const auto& lhs = m_bodies[ptr.element];

		if (lhs.shape == nullptr || lhs.collider == nullptr || !lhs.collider->GetEnabled())
			continue;

		std::vector<QuadTreeType::value_type> query;

		switch (lhs.type)
		{
		case Shape::Point:	query = m_quad_tree.Query(lhs.shape->GetCenter()); break;
		default:			query = m_quad_tree.Query(lhs.shape->GetAABB()); break;
		}

		for (QuadTreeType::value_type elt : query)
		{
			const auto& rhs = m_bodies[elt];

			if (ptr.element == elt) // skip same body
				continue;

			if (rhs.shape == nullptr || rhs.collider == nullptr || rhs.transform == nullptr) // safety checks
				continue;

			if (!rhs.collider->GetEnabled() || !lhs.collider->layer.HasAny(rhs.collider->layer)) // enabled and matching layer
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

bool BroadSystem::TryRemoveEmptyObject(uint32 index)
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
	m_entity_body_map.erase(index);

	return true;
}

void BroadSystem::RegisterEvents()
{
	m_add_ids[0].ID = m_entity_admin->RegisterOnAddListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].collider = &c;
		});

	m_add_ids[1].ID = m_entity_admin->RegisterOnAddListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].body = &pb;
		});

	m_add_ids[2].ID = m_entity_admin->RegisterOnAddListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].transform = &t;
		});

	m_mov_ids[0].ID = m_entity_admin->RegisterOnMoveListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[m_bodies_ptr[it->second].element].collider = &c;
		});

	m_mov_ids[1].ID = m_entity_admin->RegisterOnMoveListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[m_bodies_ptr[it->second].element].body = &pb;
		});

	m_mov_ids[2].ID = m_entity_admin->RegisterOnMoveListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[m_bodies_ptr[it->second].element].transform = &t;
		});

	m_rmv_ids[0].ID = m_entity_admin->RegisterOnRemoveListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[m_bodies_ptr[it->second].element].collider = nullptr;
				TryRemoveEmptyObject(it->second);
			}
		});

	m_rmv_ids[1].ID = m_entity_admin->RegisterOnRemoveListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[m_bodies_ptr[it->second].element].body = nullptr;
				TryRemoveEmptyObject(it->second);
			}
		});

	m_rmv_ids[2].ID = m_entity_admin->RegisterOnRemoveListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[m_bodies_ptr[it->second].element].transform = nullptr;
				TryRemoveEmptyObject(it->second);
			}
		});

	m_add_ids[0].ComponentID = m_entity_admin->GetComponentID<Collider>();
	m_add_ids[1].ComponentID = m_entity_admin->GetComponentID<PhysicsBody>();
	m_add_ids[2].ComponentID = m_entity_admin->GetComponentID<Transform>();

	m_mov_ids[0].ComponentID = m_entity_admin->GetComponentID<Collider>();
	m_mov_ids[1].ComponentID = m_entity_admin->GetComponentID<PhysicsBody>();
	m_mov_ids[2].ComponentID = m_entity_admin->GetComponentID<Transform>();

	m_rmv_ids[0].ComponentID = m_entity_admin->GetComponentID<Collider>();
	m_rmv_ids[1].ComponentID = m_entity_admin->GetComponentID<PhysicsBody>();
	m_rmv_ids[2].ComponentID = m_entity_admin->GetComponentID<Transform>();
}

void BroadSystem::DeregisterEvents()
{
	for (std::size_t i = 0; i < 3; ++i)
	{
		m_entity_admin->DeregisterOnAddListener(m_add_ids[i].ComponentID, m_add_ids[i].ID);
		m_entity_admin->DeregisterOnMoveListener(m_mov_ids[i].ComponentID, m_mov_ids[i].ID);
		m_entity_admin->DeregisterOnRemoveListener(m_rmv_ids[i].ComponentID, m_rmv_ids[i].ID);
	}
}
