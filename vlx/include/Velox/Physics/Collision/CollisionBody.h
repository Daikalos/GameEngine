#pragma once

#include <Velox/ECS/Identifiers.hpp>

#include "../Shapes/Shape.h"

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
	/// e.g., system for dealing with physics body and without. Then there is how the quad tree should store the collision bodies, right now it just stores indices
	/// to data in a list, which makes insertion, erasure, etc. very fast in the tree. If I were to do the other way, I would have to in some way, retrieve all
	/// the necessary data needed to represent the other object when querying the tree for nearby objects. One way is to just use the entity id, but now I have 
	/// to use get component for all components of every entity which *I'm guessing* will be kind of slow compared to other methods.
	/// 
	/// TODO: Measure performance between the two ways, currently just guessing
	/// 
	class CollisionBody
	{
	private:
		using ShapeType = typename Shape::Type;

	public:
		EntityID			entity_id;
		ShapeType			type;

		Shape*				shape;
		Collider*			collider;
		PhysicsBody*		body;
		BodyTransform*		transform;
		ColliderAABB*		aabb;
		
		ColliderEnter*		enter;
		ColliderExit*		exit;
		ColliderOverlap*	overlap;
	};
}