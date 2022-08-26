#pragma once

#include <iostream>

#include <Velox/Scene/State.hpp>
#include <Velox/Utilities.hpp>

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