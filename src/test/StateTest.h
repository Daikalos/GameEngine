#pragma once

#include <iostream>

#include "../scene/State.hpp"
#include "../utilities/Random.h"

namespace vlx
{
	class StateTest : public State
	{
	public:
		explicit StateTest(state::ID id, StateStack& state_stack, Context context)
			: State(id, state_stack, context)
		{
			
		}

		virtual bool HandleEvent(const sf::Event& event) override;
		virtual bool Update(Time& time) override;
		virtual void draw() override;

	private:

	};
}