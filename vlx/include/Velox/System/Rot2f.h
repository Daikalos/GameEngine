#pragma once

#include <SFML/System/Angle.hpp>

#include "Vector2.hpp"

namespace vlx
{
	class Rot2f
	{
	public:
		Rot2f() = default;
		explicit Rot2f(sf::Angle angle);

	public:
		Rot2f Transform(const Rot2f& rhs) const;
		Vector2f Transform(const Vector2f& rhs)const;

		Rot2f Inverse(const Rot2f& rhs) const;
		Vector2f Inverse(const Vector2f& rhs) const;

	public:
		void Set(sf::Angle angle);
		void SetIdentity();

		float GetAngle() const;

		Vector2f GetAxisX() const;
		Vector2f GetAxisY() const;

	private:
		float sin{0.0f};
		float cos{1.0f};

		friend class SimpleTransform;
	};
}