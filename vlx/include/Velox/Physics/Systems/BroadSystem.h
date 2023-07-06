#pragma once

#include <vector>
#include <unordered_map>

#include <Velox/ECS/System.hpp>
#include <Velox/ECS/EntityAdmin.h>

#include <Velox/Graphics/Components/Transform.h>

#include <Velox/Algorithms/LQuadTree.hpp>
#include <Velox/Algorithms/QTElement.hpp>
#include <Velox/Algorithms/FreeVector.hpp>

#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Point.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include <Velox/Config.hpp>
#include <Velox/Types.hpp>

#include "../PhysicsBody.h"
#include "../CollisionObject.h"
#include "../Collider.h"
#include "../ColliderEvents.h"

#include "ShapeInserter.h"

namespace vlx
{
	class VELOX_API BroadSystem final 
	{
	private:
		static constexpr int OBJ_SIZE = 6;

	public:
		using ColliderSystem		= System<Collider, Transform>;
		using ColliderSystemEvent	= SystemEvent<Collider, Transform>;

		using CollisionPair			= std::pair<uint32, uint32>;
		using CollisionList			= std::vector<CollisionPair>;

		using EntityBodyMap			= std::unordered_map<EntityID, uint32>;
		using BodyList				= std::vector<CollisionObject>;

		using QuadTreeType			= LQuadTree<QTCollider::value_type>;

	public:
		BroadSystem(EntityAdmin& entity_admin, LayerType id);
		~BroadSystem();

	public:
		void Update();

	public:
		auto GetCollisions() const noexcept -> const CollisionList&;
		auto GetCollisions() noexcept -> CollisionList&;

		const CollisionObject& GetBody(uint32 i) const noexcept;
		CollisionObject& GetBody(uint32 i) noexcept;

	private:
		void GatherPossibleCollisions();

		void CullDuplicates();

		int TryAddNewObject(EntityID eid);
		bool TryRemoveEmptyObject(uint32 index);

		void RegisterEvents();
		void DeregisterEvents();

	private:
		EntityAdmin*				m_entity_admin	{nullptr};
		LayerType					m_layer			{LYR_NONE};

		QuadTreeType				m_quad_tree;
		
		ShapeInserter<Circle>		m_circles;
		ShapeInserter<Box>			m_boxes;
		ShapeInserter<Point>		m_points;
		ShapeInserter<Polygon>		m_polygons;

		EntityBodyMap				m_entity_body_map;
		BodyList					m_bodies;			// TODO: maybe move this data to physics system

		CollisionList				m_collisions;

		std::array<int, OBJ_SIZE> m_add_ids;
		std::array<int, OBJ_SIZE> m_mov_ids;
		std::array<int, OBJ_SIZE> m_rmv_ids;
		std::array<ComponentTypeID, OBJ_SIZE> m_comp_ids;

		template<class S>
		friend class ShapeInserter;
	};
}