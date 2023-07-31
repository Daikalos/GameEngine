#pragma once

#include <SFML/System/Angle.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API GlobalTransformRotation
	{
	public:
		NODISC sf::Angle GetRotation() const;

	private:
		sf::Angle m_rotation;

		friend class GlobalTransformSystem;
	};
}