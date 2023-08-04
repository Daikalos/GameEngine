#pragma once

#include <Velox/System/Rectangle.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API ColliderAABB
	{
	public:
		const RectFloat& GetAABB() const noexcept;
		void SetAABB(const RectFloat& aabb);

	private:
		RectFloat m_aabb;
	};
}