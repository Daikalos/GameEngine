#pragma once

#include <Velox/Utilities/Rectangle.hpp>
#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	struct VELOX_API Box final : public Shape
	{
		RectFloat rectangle;

		constexpr Type GetType() const noexcept override;

		virtual void Initialize(PhysicsBody& body) const override;
		virtual RectFloat GetAABB() const override;
	};
}