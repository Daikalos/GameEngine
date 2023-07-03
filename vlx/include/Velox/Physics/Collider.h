#pragma once

#include <Velox/Algorithms/QTElement.hpp>
#include <Velox/VeloxTypes.hpp>
#include <Velox/Config.hpp>

#include "CollisionLayer.h"

#include <Velox/System/Event.hpp>
#include "CollisionResult.h"

namespace vlx
{
	using QTCollider = QTElement<uint32>;

	class VELOX_API Collider : public QTCollider
	{
	public:
		bool GetEnabled() const noexcept;
		void SetEnabled(bool flag);

	public:
		CollisionLayer layer;

	private:
		bool enabled	{true};
		bool dirty		{true}; // if should update the AABB in the quadtree

		friend class PhysicsDirtySystem;
		friend class BroadSystem;
		friend class NarrowSystem;
		friend class PhysicsSystem;
	};
}