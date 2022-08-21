#pragma once

#include "../../scene/State.hpp"
#include "../../utilities/Time.hpp"

class StateLoading final : public fge::State
{
public:
	bool HandleEvent(const sf::Event& event) override;
	bool Update(fge::Time& time) override;
	void draw() override;



};