#include "Entity.h"

void Entity::update(const float& dt)
{
	for (unsigned short i = 0; i < _components.size(); ++i)
		_components[i]->update();
}

void Entity::handle_event(const sf::Event& event)
{
	for (unsigned short i = 0; i < _components.size(); ++i)
		_components[i]->handle_event(event);
}

void Entity::draw()
{

}

void Entity::add_component(ptr component)
{
	_components.emplace_back(component);
}
