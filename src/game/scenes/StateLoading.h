#pragma once

#include "../../scene/State.hpp"
#include "../../utilities/Time.hpp"

class StateLoading final : public fge::State
{
public:
	virtual bool HandleEvent(const sf::Event& event) override;
	virtual bool Update(fge::Time& time) override;
	virtual void draw() override;

private:

}