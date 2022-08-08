#pragma once

#include <SFML/Graphics.hpp>

class Entity // switch to component-based
{
public:
	void set_velocity(sf::Vector2f velocity)
	{
		_velocity = velocity;
	}
	void set_velocity(float vx, float vy)
	{
		_velocity.x = vx;
		_velocity.y = vy;
	}
	sf::Vector2f get_velocity() const
	{
		return _velocity;
	}

private:
	sf::Vector2f _velocity;
};

