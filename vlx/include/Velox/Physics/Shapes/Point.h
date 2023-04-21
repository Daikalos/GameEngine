#pragma once

#include <Velox/Config.hpp>

#include "Shape.h"

namespace vlx
{
	class Point final : public Shape
	{
	public:
		constexpr Point();
		constexpr Point(const Vector2f& offset);
		constexpr Point(float x, float y);

	public:
		constexpr const Vector2f& GetOffset() const noexcept;
		constexpr void SetOffset(const Vector2f& offset);

	public:
		constexpr auto GetType() const noexcept -> Type override;

		VELOX_API void AdjustBody(PhysicsBody& body) const override;
		VELOX_API Vector2f ComputeCenter(const Vector2f& position) const override;
		VELOX_API RectFloat ComputeAABB(const GlobalTransform& transform) const override;

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