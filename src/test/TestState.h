#pragma once

#include "../scene/State.hpp"

#include <iostream>

namespace fge
{
	class TestState : public State
	{
	public:
		explicit TestState(States::ID id, StateStack& state_stack, Context context)
			: State(id, state_stack, context)
		{
			std::cout << "test" << std::endl;
		}

		virtual bool HandleEvent(const sf::Event& event) override;
		virtual bool Update(Time& time) override;
		virtual void draw() override;

	private:

	};
}