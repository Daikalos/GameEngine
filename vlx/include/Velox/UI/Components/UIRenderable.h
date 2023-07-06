#pragma once

#include <Velox/System/Vector2.hpp>

namespace vlx::ui
{
	class UIRenderable
	{
	public:
		using Vector2Type = Vector2<uint16>;

	public:
		auto GetSize() -> const Vector2Type&;

	private:
		Vector2Type m_size; // every ui renderable has a size
	};
}