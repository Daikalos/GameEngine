#pragma once

#include <iostream>

#include <Velox/Scene/State.hpp>
#include <Velox/Utilities.hpp>

namespace vlx
{
	class StateTest : public State<>
	{
	public:
		using State::State;

	public:
		bool HandleEvent(const sf::Event& event) override;
		bool Update(Time& time) override;
		void Draw() override;

	private:

	};
}