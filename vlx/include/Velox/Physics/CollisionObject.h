#pragma once

#include <Velox/ECS/Identifiers.hpp>

#include "Shapes/Shape.h"

namespace vlx
{
	class Collider;
	class PhysicsBody;
	class Transform;
	struct ColliderEnter;
	struct ColliderExit;
	struct ColliderOverlap;

	/// Contains the data that represents an object for collision
	/// 
	class CollisionObject
	{
	private:
		using Type = typename Shape::Type;

	public:
		EntityID		entity_id	{NULL_ENTITY};
		Type			type		{Type::None};

		Shape*			shape		{nullptr};
		Collider*		collider	{nullptr};
		PhysicsBody*	body		{nullptr};
		Transform*		transform	{nullptr};
		
		ColliderEnter*		enter	{nullptr};
		ColliderExit*		exit	{nullptr};
		ColliderOverlap*	overlap	{nullptr};

		constexpr bool operator==(const CollisionObject& rhs) const
		{
			return entity_id == rhs.entity_id;
		}
	};
}