#include <Velox/Physics/Systems/BroadSystem.h>

#include <Velox/Graphics/Components/Transform.h>
#include <Velox/ECS/EntityAdmin.h>

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

	GatherCollisions();

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

void BroadSystem::GatherCollisions()
{
	const auto QueryResult = [this](const CollisionObject& object)
	{
		switch (object.type)
		{
		case Shape::Point:
			return m_quad_tree.Query(object.shape->GetCenter());
		default:
			return m_quad_tree.Query(object.shape->GetAABB());
		}
	};

	for (std::size_t i = 0; i < m_bodies.size(); ++i)
	{
		const auto& lhs = m_bodies[i];

		if (!HasDataForCollision(lhs)) [[unlikely]] // unlikely since if you added a shape, you will likely have the other data as well
			continue;

		if (!lhs.collider->GetEnabled())
			continue;

		std::vector<QuadTreeType::value_type> query = QueryResult(lhs);

		for (const auto j : query)
		{
			const auto& rhs = m_bodies[j];

			if (lhs.entity_id == rhs.entity_id) // skip same body
				continue;

			if (!HasDataForCollision(rhs)) [[unlikely]]
				continue;

			if (!rhs.collider->GetEnabled() || !lhs.collider->layer.HasAny(rhs.collider->layer)) // enabled and matching layer
				continue;

			if (lhs.body != nullptr && rhs.body != nullptr) // if both have a physics body, do an early check to see if valid
			{
				const bool lhs_active = (lhs.body->IsAwake() && lhs.body->IsEnabled());
				const bool rhs_active = (rhs.body->IsAwake() && rhs.body->IsEnabled());

				if (!lhs_active && !rhs_active) // skip attempting collision if both are either asleep or disabled
					continue;
			}

			m_collisions.emplace_back(i, j);
		}
	}
}

void BroadSystem::CullDuplicates()
{
	std::ranges::sort(m_collisions.begin(), m_collisions.end(),
		[](const CollisionPair& x, const CollisionPair& y)
		{
			return (x.first < y.first) || (x.first == y.first && x.second < y.second);
		});

	const auto [first, last] = std::ranges::unique(m_collisions.begin(), m_collisions.end(),
		[](const CollisionPair& x, const CollisionPair& y)
		{
			return x.first == y.first && y.second == x.second;
		});

	m_collisions.erase(first, last);
}

int BroadSystem::CreateBody(EntityID eid, Shape* shape, typename Shape::Type type)
{
	assert(!m_entity_body_map.contains(eid));

	CollisionObject& object = m_bodies.emplace_back(eid, type);

	object.shape		= shape;
	object.collider		= m_entity_admin->TryGetComponent<Collider>(eid);
	object.body			= m_entity_admin->TryGetComponent<PhysicsBody>(eid);
	object.transform	= m_entity_admin->TryGetComponent<Transform>(eid);

	object.enter		= m_entity_admin->TryGetComponent<ColliderEnter>(eid);
	object.exit			= m_entity_admin->TryGetComponent<ColliderExit>(eid);
	object.overlap		= m_entity_admin->TryGetComponent<ColliderOverlap>(eid);

	return m_entity_body_map.try_emplace(eid, m_bodies.size() - 1).first->second;
}

int BroadSystem::FindBody(EntityID eid)
{
	if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
		return it->second;

	return NULL_BODY;
}

void BroadSystem::RemoveBody(EntityID eid)
{
	const auto it1 = m_entity_body_map.find(eid);
	assert(it1 != m_entity_body_map.end() && "Entity should be in the map");

	const auto it2 = m_entity_body_map.find(m_bodies.back().entity_id);
	assert(it2 != m_entity_body_map.end() && "Entity should be in the map");

	if (m_bodies[it2->second].collider)
		m_bodies[it2->second].collider->Update(it1->second); // update the index in the quad tree

	it2->second = it1->second;

	cu::SwapPopAt(m_bodies, it1->second);
	m_entity_body_map.erase(it1);
}

bool BroadSystem::HasDataForCollision(const CollisionObject& object)
{
	return object.shape != nullptr && object.collider != nullptr && object.transform != nullptr; // safety checks
}

void BroadSystem::RegisterEvents()
{
	m_add_ids[0] = m_entity_admin->RegisterOnAddListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].collider = &c;
		});

	m_add_ids[1] = m_entity_admin->RegisterOnAddListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				 m_bodies[i].body = &pb;
		});

	m_add_ids[2] = m_entity_admin->RegisterOnAddListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].transform = &t;
		});

	m_add_ids[3] = m_entity_admin->RegisterOnAddListener<ColliderEnter>(
		[this](EntityID eid, ColliderEnter& e)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].enter = &e;
		});

	m_add_ids[4] = m_entity_admin->RegisterOnAddListener<ColliderExit>(
		[this](EntityID eid, ColliderExit& e)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].exit = &e;
		});

	m_add_ids[5] = m_entity_admin->RegisterOnAddListener<ColliderOverlap>(
		[this](EntityID eid, ColliderOverlap& o)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].overlap = &o;
		});

	m_mov_ids[0] = m_entity_admin->RegisterOnMoveListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].collider = &c;
		});

	m_mov_ids[1] = m_entity_admin->RegisterOnMoveListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].body = &pb;
		});

	m_mov_ids[2] = m_entity_admin->RegisterOnMoveListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].transform = &t;
		});

	m_mov_ids[3] = m_entity_admin->RegisterOnMoveListener<ColliderEnter>(
		[this](EntityID eid, ColliderEnter& e)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].enter = &e;
		});

	m_mov_ids[4] = m_entity_admin->RegisterOnMoveListener<ColliderExit>(
		[this](EntityID eid, ColliderExit& e)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].exit = &e;
		});

	m_mov_ids[5] = m_entity_admin->RegisterOnMoveListener<ColliderOverlap>(
		[this](EntityID eid, ColliderOverlap& o)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].overlap = &o;
		});

	m_rmv_ids[0] = m_entity_admin->RegisterOnRemoveListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].collider = nullptr;
		});

	m_rmv_ids[1] = m_entity_admin->RegisterOnRemoveListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].body = nullptr;
		});

	m_rmv_ids[2] = m_entity_admin->RegisterOnRemoveListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].transform = nullptr;
		});

	m_rmv_ids[3] = m_entity_admin->RegisterOnRemoveListener<ColliderEnter>(
		[this](EntityID eid, ColliderEnter& e)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].enter = nullptr;
		});

	m_rmv_ids[4] = m_entity_admin->RegisterOnRemoveListener<ColliderExit>(
		[this](EntityID eid, ColliderExit& e)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].exit = nullptr;
		});

	m_rmv_ids[5] = m_entity_admin->RegisterOnRemoveListener<ColliderOverlap>(
		[this](EntityID eid, ColliderOverlap& o)
		{
			if (auto i = FindBody(eid); i != NULL_BODY)
				m_bodies[i].overlap = nullptr;
		});

	m_comp_ids[0] = m_entity_admin->GetComponentID<Collider>();
	m_comp_ids[1] = m_entity_admin->GetComponentID<PhysicsBody>();
	m_comp_ids[2] = m_entity_admin->GetComponentID<Transform>();
	m_comp_ids[3] = m_entity_admin->GetComponentID<ColliderEnter>();
	m_comp_ids[4] = m_entity_admin->GetComponentID<ColliderExit>();
	m_comp_ids[5] = m_entity_admin->GetComponentID<ColliderOverlap>();
}

void BroadSystem::DeregisterEvents()
{
	for (std::size_t i = 0; i < OBJ_SIZE; ++i)
	{
		m_entity_admin->DeregisterOnAddListener(m_comp_ids[i], m_add_ids[i]);
		m_entity_admin->DeregisterOnMoveListener(m_comp_ids[i], m_mov_ids[i]);
		m_entity_admin->DeregisterOnRemoveListener(m_comp_ids[i], m_rmv_ids[i]);
	}
}
