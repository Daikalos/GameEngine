#pragma once

#include <Velox/System/Vector2.hpp>

#include <Velox/Config.hpp>

namespace vlx::ui
{
	class VELOX_API UIBase
	{
	public:
		using Vector2Type = Vector2<uint16>;

	public:
		auto GetSize() const -> const Vector2Type&;

		void SetSize(const Vector2Type& size);

	private:
		Vector2Type m_size; // every ui renderable has a size
	};
}