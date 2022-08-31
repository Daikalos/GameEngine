#include "StateTest.h"

using namespace vlx;

bool StateTest::HandleEvent(const sf::Event& event)
{
    return true;
}

bool StateTest::Update(Time& time)
{
    return true;
}

void StateTest::draw()
{
    sf::CircleShape circle;
    circle.setPosition(sf::Vector2f(250.0f, 250.0f));
    circle.setRadius(100.0f);
    circle.setFillColor(sf::Color::Red);
    GetContext().window->draw(circle);
}
