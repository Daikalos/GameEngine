#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "Component.h"

class Entity // switch to component-based
{
public:
	using ptr = std::unique_ptr<Component>;

public:
	void update(const float& dt);
	void handle_event(const sf::Event& event);
	void draw();

	void add_component(ptr component);

private:
	std::vector<ptr> _components;
};

