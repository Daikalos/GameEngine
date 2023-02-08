#include <Velox/Components/Velocity.h>

using namespace vlx;

constexpr Velocity::Velocity() : velocity(0, 0) {}
constexpr Velocity::Velocity(const float x, const float y) : velocity(x, y) {}
constexpr Velocity::Velocity(const sf::Vector2f& vel) : velocity(vel) {}