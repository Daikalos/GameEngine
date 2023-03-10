#pragma once

namespace vlx
{
	class Shape;
	class Collision;
	class LocalTransform;
	class Transform;

	/// Contains the necessary data to represent an object for collision
	/// 
	class CollisionObject
	{
	public:
		Shape*			Shape			{nullptr};
		Collision*		Collision		{nullptr};
		LocalTransform* LocalTransform	{nullptr};
		Transform*		Transform		{nullptr};

		constexpr bool operator==(const CollisionObject& rhs) const
		{
			return Shape == rhs.Shape && Collision == rhs.Collision && LocalTransform == rhs.LocalTransform && Transform == rhs.Transform;
		}
	};
}