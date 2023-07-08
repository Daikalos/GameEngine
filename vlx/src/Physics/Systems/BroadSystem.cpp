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
	for (std::size_t i = 0; i < m_bodies.size(); ++i)
	{
		const auto& lhs = m_bodies[i];

		if (lhs.shape == nullptr || lhs.collider == nullptr || lhs.transform == nullptr) // safety checks
			continue;

		if (!lhs.collider->GetEnabled())
			continue;

		std::vector<QuadTreeType::value_type> query;

		switch (lhs.type)
		{
		case Shape::Point:	query = m_quad_tree.Query(lhs.shape->GetCenter()); break;
		default:			query = m_quad_tree.Query(lhs.shape->GetAABB()); break;
		}

		for (std::size_t j = 0; j < query.size(); ++j)
		{
			const auto elt = query[j];
			const auto& rhs = m_bodies[elt];

			if (i == elt) // skip same body
				continue;

			if (rhs.shape == nullptr || rhs.collider == nullptr || rhs.transform == nullptr)
				continue;

			if (!rhs.collider->GetEnabled() || !lhs.collider->layer.HasAny(rhs.collider->layer)) // enabled and matching layer
				continue;

			if (lhs.body != nullptr && rhs.body != nullptr) // if both are physics body, do an early check to see if valid
			{
				bool lhs_active = (lhs.body->IsAwake() && lhs.body->IsEnabled());
				bool rhs_active = (rhs.body->IsAwake() && rhs.body->IsEnabled());

				if (!lhs_active && !rhs_active) // skip attempting collision if both are either asleep or disabled
					continue;
			}

			m_collisions.emplace_back(i, elt);
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

int BroadSystem::TryAddNewObject(EntityID eid)
{
	const auto it = m_entity_body_map.find(eid);
	if (it == m_entity_body_map.end())
	{
		m_bodies.emplace_back(eid);
		m_entity_body_map.emplace(eid, m_bodies.size() - 1);

		return static_cast<int>(m_bodies.size() - 1);
	}

	return it->second;
}

bool BroadSystem::TryRemoveEmptyObject(uint32 index)
{
	const CollisionObject& object = m_bodies[index];

	if (object.body || object.collider || object.shape || object.enter || object.exit || object.overlap)
		return false;

	const auto it = m_entity_body_map.find(m_bodies.back().entity_id);
	assert(it != m_entity_body_map.end() && "Entity should be in the map");

	if (m_bodies[it->second].collider)
		m_bodies[it->second].collider->Update(index); // update the index in the quad tree

	it->second = index;

	m_entity_body_map.erase(object.entity_id);
	cu::SwapPopAt(m_bodies, index);

	return true;
}

void BroadSystem::RegisterEvents()
{
	m_add_ids[0] = m_entity_admin->RegisterOnAddListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].collider = &c;
		});

	m_add_ids[1] = m_entity_admin->RegisterOnAddListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].body = &pb;
		});

	m_add_ids[2] = m_entity_admin->RegisterOnAddListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].transform = &t;
		});

	m_add_ids[3] = m_entity_admin->RegisterOnAddListener<ColliderEnter>(
		[this](EntityID eid, ColliderEnter& e)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].enter = &e;
		});

	m_add_ids[4] = m_entity_admin->RegisterOnAddListener<ColliderExit>(
		[this](EntityID eid, ColliderExit& e)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].exit = &e;
		});

	m_add_ids[5] = m_entity_admin->RegisterOnAddListener<ColliderOverlap>(
		[this](EntityID eid, ColliderOverlap& o)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].overlap = &o;
		});

	m_mov_ids[0] = m_entity_admin->RegisterOnMoveListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[it->second].collider = &c;
		});

	m_mov_ids[1] = m_entity_admin->RegisterOnMoveListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[it->second].body = &pb;
		});

	m_mov_ids[2] = m_entity_admin->RegisterOnMoveListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[it->second].transform = &t;
		});

	m_mov_ids[3] = m_entity_admin->RegisterOnMoveListener<ColliderEnter>(
		[this](EntityID eid, ColliderEnter& e)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[it->second].enter = &e;
		});

	m_mov_ids[4] = m_entity_admin->RegisterOnMoveListener<ColliderExit>(
		[this](EntityID eid, ColliderExit& e)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[it->second].exit = &e;
		});

	m_mov_ids[5] = m_entity_admin->RegisterOnMoveListener<ColliderOverlap>(
		[this](EntityID eid, ColliderOverlap& o)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
				m_bodies[it->second].overlap = &o;
		});

	m_rmv_ids[0] = m_entity_admin->RegisterOnRemoveListener<Collider>(
		[this](EntityID eid, Collider& c)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[it->second].collider = nullptr;
				TryRemoveEmptyObject(it->second);
			}
		});

	m_rmv_ids[1] = m_entity_admin->RegisterOnRemoveListener<PhysicsBody>(
		[this](EntityID eid, PhysicsBody& pb)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[it->second].body = nullptr;
				TryRemoveEmptyObject(it->second);
			}
		});

	m_rmv_ids[2] = m_entity_admin->RegisterOnRemoveListener<Transform>(
		[this](EntityID eid, Transform& t)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[it->second].transform = nullptr;
				TryRemoveEmptyObject(it->second);
			}
		});

	m_rmv_ids[3] = m_entity_admin->RegisterOnRemoveListener<ColliderEnter>(
		[this](EntityID eid, ColliderEnter& e)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[it->second].enter = nullptr;
				TryRemoveEmptyObject(it->second);
			}
		});

	m_rmv_ids[4] = m_entity_admin->RegisterOnRemoveListener<ColliderExit>(
		[this](EntityID eid, ColliderExit& e)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[it->second].exit = nullptr;
				TryRemoveEmptyObject(it->second);
			}
		});

	m_rmv_ids[5] = m_entity_admin->RegisterOnRemoveListener<ColliderOverlap>(
		[this](EntityID eid, ColliderOverlap& o)
		{
			if (auto it = m_entity_body_map.find(eid); it != m_entity_body_map.end())
			{
				m_bodies[it->second].overlap = nullptr;
				TryRemoveEmptyObject(it->second);
			}
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
