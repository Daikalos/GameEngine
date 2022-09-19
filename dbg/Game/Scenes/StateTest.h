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
		virtual bool HandleEvent(const sf::Event& event) override;
		virtual bool Update(Time& time) override;
		virtual void draw() override;

	private:

	};
}