#pragma once

#include "Shapes/Shape.h"

namespace vlx
{
	class Collision;
	class PhysicsBody;
	class LocalTransform;
	class Transform;

	/// Contains the data that represents an object for collision
	/// 
	class CollisionObject
	{
	private:
		using Type = typename Shape::Type;

	public:
		EntityID		entity_id		{NULL_ENTITY};
		Type			type			{-1};
		Shape*			shape			{nullptr};
		Collision*		collision		{nullptr};
		PhysicsBody*	body			{nullptr};
		LocalTransform* local_transform	{nullptr};

		constexpr bool operator==(const CollisionObject& rhs) const
		{
			return entity_id == rhs.entity_id;
		}
	};
}