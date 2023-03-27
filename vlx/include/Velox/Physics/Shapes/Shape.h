#pragma once

#include <Velox/ECS/IComponent.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/System/Rectangle.hpp>
#include <Velox/System/Event.hpp>
#include <Velox/Config.hpp>

#include "../CollisionResult.h"
#include "../PhysicsBody.h"

namespace vlx
{
	class VELOX_API Shape : public IComponent
	{
	public:
		enum Type : short
		{
			Circle,
			Box,
			Point,
			Convex,

			// Not sure if the engine will support concave shapes, seems very difficult to implement and may cost a lot of performance and memory,
			// http://wscg.zcu.cz/wscg2004/Papers_2004_Full/B83.pdf is the best i've found that presents a viable solution

			Count // always keep last
		};

		virtual ~Shape() = default;

		virtual constexpr Type GetType() const noexcept = 0;
		virtual void SetAABB(const Transform& transform) = 0;
		virtual void Initialize(PhysicsBody& body) const = 0;

	public:
		const RectFloat& GetAABB() const;
		Vector2f GetCenter() const;

	protected:
		RectFloat m_aabb;
	};
}