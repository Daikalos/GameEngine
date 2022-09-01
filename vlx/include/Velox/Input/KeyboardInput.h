#pragma once

#include "InputHandler.h"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Handles all of the keyboard input, only supports one
	// keyboard at a time
	////////////////////////////////////////////////////////////
	class VELOX_API KeyboardInput : public InputHandler
	{
	public:
		void Update(const Time& time, const bool focus) override;
		void HandleEvent(const sf::Event& event) override;

	public:
		bool Held(const sf::Keyboard::Key key) const;
		bool Pressed(const sf::Keyboard::Key key) const;
		bool Released(const sf::Keyboard::Key key) const;

	private:
		bool	m_current_state		[sf::Keyboard::KeyCount] = {false};
		bool	m_previous_state	[sf::Keyboard::KeyCount] = {false};
		float	m_held_time			[sf::Keyboard::KeyCount] = {0.0f};
	};
}