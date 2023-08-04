#pragma once

#include <Velox/ECS/Identifiers.hpp>

#include "Shapes/Shape.h"

namespace vlx
{
	class Collider;
	class PhysicsBody;
	class BodyTransform;
	class ColliderAABB;
	struct ColliderEnter;
	struct ColliderExit;
	struct ColliderOverlap;

	/// Contains the needed data for an object when colliding
	/// 
	/// TODO: maybe remove this due to pointer access being relatively expensive (bodies data storage in broad system makes it so that the data is accessed
	/// in random order, likely causing poor performance from cache misses). Did use ECS before but felt it was too annoying with dealing with every case, 
	/// e.g., system for dealing with physics body and without.
	/// 
	/// TODO: Measure performance between the two ways, currently just guessing
	/// 
	class CollisionBody
	{
	private:
		using ShapeType = typename Shape::Type;

	public:
		EntityID		entity_id	{NULL_ENTITY};
		ShapeType		type		{ShapeType::None};

		Shape*			shape		{nullptr};
		Collider*		collider	{nullptr};
		PhysicsBody*	body		{nullptr};
		BodyTransform*	transform	{nullptr};
		ColliderAABB*	aabb		{nullptr};
		
		ColliderEnter*		enter	{nullptr};
		ColliderExit*		exit	{nullptr};
		ColliderOverlap*	overlap	{nullptr};

		constexpr bool operator==(const CollisionBody& rhs) const
		{
			return entity_id == rhs.entity_id;
		}
	};
}