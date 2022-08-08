#pragma once

#include <SFML/Graphics.hpp>

class Component
{
public:
	virtual void update() = 0;
	virtual void handle_event(const sf::Event& event) = 0;
	virtual void draw() = 0;

	virtual size_t get_id() = 0;

private:
};

