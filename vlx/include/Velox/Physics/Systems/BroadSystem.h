#pragma once

#include <vector>
#include <unordered_map>

#include <Velox/Algorithms/LQuadTree.hpp>

#include <Velox/Physics/Shapes/Shape.h>
#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Point.h>
#include <Velox/Physics/Shapes/Polygon.h>

#include <Velox/Config.hpp>
#include <Velox/Types.hpp>

#include "ShapeInserter.h"

namespace vlx
{
	class EntityAdmin;
	class CollisionBody;

	class VELOX_API BroadSystem final 
	{
	private:
		static constexpr int NULL_BODY = -1;

	public:
		using CollisionPair			= std::pair<uint32, uint32>;
		using CollisionList			= std::vector<CollisionPair>;

		using EntityBodyMap			= std::unordered_map<EntityID, uint32>;
		using BodyList				= std::vector<CollisionBody>;

		using QuadTreeType			= LQuadTree<typename QTBody::ValueType>;

	public:
		BroadSystem(EntityAdmin& entity_admin, LayerType id);
		~BroadSystem();

	public:
		void Update();

	public:
		auto GetCollisions() const noexcept -> const CollisionList&;
		auto GetCollisions() noexcept -> CollisionList&;

		const CollisionBody& GetBody(uint32 i) const noexcept;
		CollisionBody& GetBody(uint32 i) noexcept;

	private:
		void GatherCollisions();
		void CullDuplicates();

		int CreateBody(EntityID eid, Shape* shape, typename Shape::Type type);
		int FindBody(EntityID eid);
		void RemoveBody(EntityID eid);

		static bool HasDataForCollision(const CollisionBody& body);

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
		BodyList					m_bodies;

		CollisionList				m_collisions;

		std::vector<EventID>		m_event_ids;

		template<std::derived_from<Shape> S>
		friend class ShapeInserter;
	};
}