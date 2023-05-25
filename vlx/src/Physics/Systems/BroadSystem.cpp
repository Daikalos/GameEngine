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

	m_add_coll_id = entity_admin.RegisterOnAddListener<Collider>([this](EntityID eid, Collider& c)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].collider = &c;
		});

	m_add_body_id = entity_admin.RegisterOnAddListener<PhysicsBody>([this](EntityID eid, PhysicsBody& pb)
		{
			int i = TryAddNewObject(eid);
			m_bodies[i].body = &pb;
		});

	m_mov_coll_id = entity_admin.RegisterOnMoveListener<Collider>([this](EntityID eid, Collider& c)
		{
			const auto it = m_entity_body_map.find(eid);
			if (it != m_entity_body_map.end())
				m_bodies[it->second].collider = &c;
		});

	m_mov_body_id = entity_admin.RegisterOnMoveListener<PhysicsBody>([this](EntityID eid, PhysicsBody& pb)
		{
			const auto it = m_entity_body_map.find(eid);
			if (it != m_entity_body_map.end())
				m_bodies[it->second].body = &pb;
		});

	m_rmv_coll_id = entity_admin.RegisterOnRemoveListener<Collider>([this](EntityID eid, Collider& c)
		{
			const auto it = m_entity_body_map.find(eid);
			if (it != m_entity_body_map.end())
			{
				m_bodies[it->second].collider = nullptr;
				if (RemoveEmptyObject(it->second))
					m_entity_body_map.erase(it);
			}
		});

	m_rmv_body_id = entity_admin.RegisterOnRemoveListener<PhysicsBody>([this](EntityID eid, PhysicsBody& pb)
		{
			const auto it = m_entity_body_map.find(eid);
			if (it != m_entity_body_map.end())
			{
				m_bodies[it->second].body = nullptr;
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

int BroadSystem::TryAddNewObject(EntityID eid)
{
	const auto it = m_entity_body_map.find(eid);
	if (it == m_entity_body_map.end())
	{
		const auto i = m_bodies.emplace(eid);
		m_entity_body_map.emplace(eid, i);

		return i;
	}

	return it->second;
}

bool BroadSystem::RemoveEmptyObject(uint32 index)
{
	CollisionObject& object = m_bodies[index];

	if (object.body || object.collider || object.shape)
		return false;

	m_bodies.erase(index);

	return true;
}
