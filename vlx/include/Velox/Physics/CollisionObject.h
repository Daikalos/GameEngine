#pragma once

#include "Shapes/Shape.h"

namespace vlx
{
	class Collider;
	class PhysicsBody;
	class Transform;

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

		constexpr bool operator==(const CollisionObject& rhs) const
		{
			return entity_id == rhs.entity_id;
		}
	};
}