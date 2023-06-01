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

#include "ShapeInserter.h"

namespace vlx
{
	class VELOX_API BroadSystem final 
	{
	private:
		struct BodyPtr
		{
			int element {-1};	
			int next	{-1};	
			int prev	{-1};
		};

	public:
		using GeneralSystem		= System<Collider, Transform>;

		using CollisionPair		= std::pair<uint32, uint32>;
		using CollisionList		= std::vector<CollisionPair>;

		using EntityBodyMap		= std::unordered_map<EntityID, uint32>;
		using QuadTree			= LQuadTree<QTCollider::value_type>;

	public:
		BroadSystem(EntityAdmin& entity_admin, LayerType id);
		~BroadSystem();

	public:
		void Update();

	public:
		auto GetCollisions() const noexcept -> std::span<const CollisionPair>;
		auto GetCollisions() noexcept -> std::span<CollisionPair>;

		const CollisionObject& GetBody(uint32 i) const noexcept;
		CollisionObject& GetBody(uint32 i) noexcept;

	private:
		void CullDuplicates();

		int TryAddNewObject(EntityID eid);
		bool RemoveEmptyObject(uint32 index);

	private:
		EntityAdmin*				m_entity_admin	{nullptr};
		LayerType					m_layer			{LYR_NONE};

		QuadTree					m_quad_tree;
		
		ShapeInserter<Circle>		m_circles;
		ShapeInserter<Box>			m_boxes;
		ShapeInserter<Point>		m_points;

		GeneralSystem				m_cleanup;

		EntityBodyMap				m_entity_body_map;
		FreeVector<CollisionObject>	m_bodies;
		FreeVector<BodyPtr>			m_bodies_ptr;
		CollisionList				m_pairs;
		int							m_first_body	{-1};

		int							m_add_coll_id	{-1};
		int							m_add_body_id	{-1};
		int							m_mov_body_id	{-1};
		int							m_mov_coll_id	{-1};
		int							m_rmv_body_id	{-1};
		int							m_rmv_coll_id	{-1};

		template<class S>
		friend class ShapeInserter;
	};
}