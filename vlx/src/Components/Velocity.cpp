#include <Velox/Components/Velocity.h>

using namespace vlx;

Velocity::Velocity() : velocity(0, 0) 
{

}
Velocity::Velocity(const sf::Vector2f& vel) : velocity(vel) 
{

}