#pragma once

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics.hpp>

#include "State.h"

namespace fge
{
	class StateStack : private sf::NonCopyable
	{
	public:

	private:
		std::vector<State::ptr> _stack;
		std::
	};
}