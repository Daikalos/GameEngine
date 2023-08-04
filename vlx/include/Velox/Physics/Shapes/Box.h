#pragma once

#include <array>

#include <Velox/Utility/ArithmeticUtils.h>
#include <Velox/System/Mat4f.hpp>
#include <Velox/System/Rectangle.hpp>
#include <Velox/System/Vector2.hpp>
#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class Box final : public Shape
	{
	private:
		using VectorArray = std::array<Vector2f, 4>;

	public:
		static constexpr VectorArray NORMALS =
		{
			{{	 0.0f,	-1.0f	},
			{	 1.0f,	 0.0f	},
			{	 0.0f,	 1.0f	},
			{	-1.0f,	 0.0f	}}
		};

	public:
		constexpr Box();
		constexpr Box(const RectFloat& rect);
		constexpr Box(const Vector2f& min, const Vector2f& max);
		constexpr Box(float min_x, float min_y, float max_x, float max_y);
		constexpr Box(const Vector2f& size);
		constexpr Box(float width, float height);

	public:
		constexpr auto GetVertices() const noexcept -> const VectorArray&;

		constexpr RectFloat GetBox() const noexcept;
		constexpr Vector2f GetSize() const noexcept;

		constexpr float GetWidth() const noexcept;
		constexpr float GetHeight() const noexcept;

		constexpr void SetBox(const RectFloat& box);
		constexpr void SetSize(const Vector2f& size);
		
		constexpr void SetLeft(float left);
		constexpr void SetTop(float top);
		constexpr void SetRight(float right);
		constexpr void SetBottom(float bottom);

	public:
		constexpr auto GetType() const noexcept -> Type override;
		VELOX_API void AdjustBody(PhysicsBody& body) const override;

	public:
		VELOX_API RectFloat ComputeAABB(const Mat4f& matrix) const;

	private:
		VectorArray m_vertices;
	};

	constexpr Box::Box() {};
	constexpr Box::Box(const RectFloat& box)
	{
		SetBox(box);
	}
	constexpr Box::Box(const Vector2f& min, const Vector2f& max)
	{
		SetBox({ min, max });
	}
	constexpr Box::Box(float min_x, float min_y, float max_x, float max_y)
	{
		SetBox({ min_x, min_y, max_x, max_y });
	}
	constexpr Box::Box(const Vector2f& size)
	{
		SetSize(size);
	}
	constexpr Box::Box(float width, float height)
	{
		SetSize({ width, height });
	}

	constexpr auto Box::GetVertices() const noexcept -> const VectorArray&
	{
		return m_vertices;
	}

	constexpr RectFloat Box::GetBox() const noexcept
	{
		return RectFloat({}, m_vertices[2] - m_vertices[0]);
	}
	constexpr Vector2f Box::GetSize() const noexcept
	{
		return { GetWidth(), GetHeight() };
	}

	constexpr float Box::GetWidth() const noexcept
	{
		return au::Abs(m_vertices[1].x - m_vertices[0].x);
	}

	constexpr float Box::GetHeight() const noexcept
	{
		return au::Abs(m_vertices[3].y - m_vertices[0].y);
	}

	constexpr void Box::SetBox(const RectFloat& box)
	{
		m_vertices[0] = Vector2f(box.left,		box.top);
		m_vertices[1] = Vector2f(box.Right(),	box.top);
		m_vertices[2] = Vector2f(box.Right(),	box.Bottom());
		m_vertices[3] = Vector2f(box.left,		box.Bottom());
	}
	constexpr void Box::SetSize(const Vector2f& size)
	{
		SetLeft(-size.x / 2.0f);
		SetTop(-size.y / 2.0f);
		SetRight(size.x);
		SetBottom(size.y);
	}

	constexpr void Box::SetLeft(float left)
	{
		m_vertices[0].x = left;
		m_vertices[3].x = left;
	}
	constexpr void Box::SetTop(float top)
	{
		m_vertices[0].y = top;
		m_vertices[1].y = top;
	}
	constexpr void Box::SetRight(float right)
	{
		m_vertices[1].x = m_vertices[0].x + right;
		m_vertices[2].x = m_vertices[0].x + right;
	}
	constexpr void Box::SetBottom(float bottom)
	{
		m_vertices[2].y = m_vertices[0].y + bottom;
		m_vertices[3].y = m_vertices[0].y + bottom;
	}

	constexpr Shape::Type Box::GetType() const noexcept
	{
		return Shape::Box;
	}
}