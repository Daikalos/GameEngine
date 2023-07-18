#pragma once

#include <SFML/System/Angle.hpp>

#include "Rot2f.h"
#include "Vector2.hpp"

namespace vlx
{
	/// Simple transformation without origin and scaling
	/// 
	class SimpleTransform
	{
	public:
		SimpleTransform() = default;
		SimpleTransform(const Vector2f& position, const Rot2f& rotation);

	public:
		Vector2f Transform(const Vector2f& rhs) const;
		Vector2f Inverse(const Vector2f& rhs) const;

		SimpleTransform Transform(const SimpleTransform& rhs) const;
		SimpleTransform Inverse(const SimpleTransform& rhs) const;

	public:
		void SetIdentity();

		void Set(const Vector2f& position, sf::Angle angle);

		void SetPosition(const Vector2f& position);
		void SetRotation(sf::Angle angle);

		const Vector2f& GetPosition() const noexcept;
		const Rot2f& GetRotation() const noexcept;

	private:
		Vector2f	m_pos;
		Rot2f		m_rot;
	};
}