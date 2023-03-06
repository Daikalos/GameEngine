#pragma once

#include <array>

#include <Velox/Graphics/Rectangle.hpp>
#include <Velox/Graphics/Vector2.hpp>
#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class VELOX_API Box final : public Shape
	{
	private:
		using VecArr = std::array<Vector2f, 4>;

	public:
		static constexpr VecArr NORMALS =
		{
			{{	 0.0f,	-1.0f	},
			{	 1.0f,	 0.0f	},
			{	 0.0f,	 1.0f	},
			{	-1.0f,	 0.0f	}}
		};

	public:
		Box() = default;
		Box(const RectFloat& rect);
		Box(const Vector2f& min, const Vector2f& max);
		Box(float min_x, float min_y, float max_x, float max_y);

	public:
		auto GetVertices() const noexcept -> const VecArr&;
		RectFloat GetBox() const noexcept;

		float GetWidth() const noexcept;
		float GetHeight() const noexcept;

		void SetBox(const RectFloat& box);
		
		void SetLeft(float left);
		void SetTop(float top);
		void SetRight(float right);
		void SetBottom(float bottom);

	public:
		constexpr Type GetType() const noexcept override;

		virtual void Initialize(PhysicsBody& body) const override;
		virtual RectFloat GetAABB() const override;

	private:
		VecArr m_vertices;
	};
}