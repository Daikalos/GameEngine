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
#include <Velox/VeloxTypes.hpp>

#include "../PhysicsBody.h"
#include "../CollisionObject.h"
#include "../Collider.h"

#include "BroadShapeHelper.h"

namespace vlx
{
	class VELOX_API BroadSystem final 
	{
	public:
		using GeneralSystem			= System<Collider, Transform>;

		using CollisionPair			= std::pair<CollisionObject, CollisionObject>;
		using CollisionList			= std::vector<CollisionPair>;
		using CollisionIndices		= std::vector<uint32>;

		using EntityBodyMap			= std::unordered_map<EntityID, uint32>;
		using QuadTree				= LQuadTree<QTCollider::value_type>;

	public:
		BroadSystem(EntityAdmin& entity_admin, LayerType id);
		~BroadSystem();

	public:
		void Update();

	public:
		auto GetPairs() const noexcept -> std::span<const CollisionPair>;
		auto GetIndices() const noexcept -> std::span<const uint32>;

		auto GetPairs() noexcept -> std::span<CollisionPair>;
		auto GetIndices() noexcept -> std::span<uint32>;

	private:
		void CullDuplicates();

		int TryAddNewObject(EntityID eid);
		bool RemoveEmptyObject(uint32 index);

	private:
		EntityAdmin*				m_entity_admin	{nullptr};
		LayerType					m_layer			{LYR_NONE};

		QuadTree					m_quad_tree;
		FreeVector<CollisionObject>	m_bodies;
		EntityBodyMap				m_entity_body_map;
		
		CollisionList				m_pairs;
		CollisionIndices			m_indices;

		BroadShapeHelper<Circle>	m_circles;
		BroadShapeHelper<Box>		m_boxes;
		BroadShapeHelper<Point>		m_points;

		GeneralSystem				m_cleanup;

		int							m_add_coll_id	{-1};
		int							m_add_body_id	{-1};
		int							m_mov_body_id	{-1};
		int							m_mov_coll_id	{-1};
		int							m_rmv_body_id	{-1};
		int							m_rmv_coll_id	{-1};

		template<class S>
		friend class BroadShapeHelper;
	};
}