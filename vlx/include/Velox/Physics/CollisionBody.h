#pragma once

#include <Velox/ECS/Identifiers.hpp>

#include "Shapes/Shape.h"

namespace vlx
{
	class Collider;
	class PhysicsBody;
	class BodyTransform;
	struct ColliderEnter;
	struct ColliderExit;
	struct ColliderOverlap;

	/// Contains the needed data for an object when colliding
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
		
		ColliderEnter*		enter	{nullptr};
		ColliderExit*		exit	{nullptr};
		ColliderOverlap*	overlap	{nullptr};

		constexpr bool operator==(const CollisionBody& rhs) const
		{
			return entity_id == rhs.entity_id;
		}
	};
}