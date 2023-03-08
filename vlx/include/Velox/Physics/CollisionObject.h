#pragma once

namespace vlx
{
	class Shape;
	class Collision;
	class LocalTransform;
	class Transform;

	/// Contains the necessary data to represent an object for collision
	/// 
	struct CollisionObject
	{
		Shape*			Shape			{nullptr};
		Collision*		Collision		{nullptr};
		LocalTransform* LocalTransform	{nullptr};
		Transform*		Transform		{nullptr};
	};
}