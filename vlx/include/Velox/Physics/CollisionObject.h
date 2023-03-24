#pragma once

namespace vlx
{
	class Shape;
	class Collision;
	class PhysicsBody;
	class LocalTransform;
	class Transform;

	/// Contains the data that represents an object for collision
	/// 
	class CollisionObject
	{
	public:
		EntityID		entity_id		{NULL_ENTITY};
		Shape*			shape			{nullptr};
		Collision*		collision		{nullptr};
		PhysicsBody*	body			{nullptr};
		LocalTransform* local_transform	{nullptr};
		Transform*		transform		{nullptr};

		constexpr bool operator==(const CollisionObject& rhs) const
		{
			return entity_id == rhs.entity_id;
		}
	};
}