#pragma once

#include "Shape.h"

#include <Velox/Config.hpp>

namespace vlx
{
	class Point final : public ShapeCRTP<Point>
	{
	public:
		constexpr Point();
		constexpr Point(const Vector2f& offset);
		constexpr Point(float x, float y);

	public:
		constexpr const Vector2f& GetOffset() const noexcept;
		constexpr void SetOffset(const Vector2f& offset);

	public:
		constexpr auto GetType() const noexcept -> Type;
		VELOX_API void InitializeImpl(PhysicsBody& body) const;
		VELOX_API void UpdateAABBImpl(const Transform& transform);

	private:
		Vector2f m_offset;
	};

	constexpr Point::Point() {};
	constexpr Point::Point(const Vector2f& offset) : m_offset(offset) {}
	constexpr Point::Point(float x, float y) : m_offset(x, y) {}

	constexpr const Vector2f& Point::GetOffset() const noexcept 
	{
		return m_offset;
	}
	constexpr void Point::SetOffset(const Vector2f& offset) 
	{
		m_offset = offset;
	}

	constexpr auto Point::GetType() const noexcept -> Type
	{
		return Type::Point;
	}
}