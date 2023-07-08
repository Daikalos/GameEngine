#pragma once

#include <SFML/Window/Keyboard.hpp>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "InputHandler.h"

namespace vlx
{
	/// Handles all of the keyboard input, only supports one keyboard 
	///
	class VELOX_API KeyboardInput final : public InputHandler
	{
	public:
		using ButtonType = sf::Keyboard::Key;

	public:
		KeyboardInput() = default;

	public:
		NODISC bool Held(ButtonType key) const;
		NODISC bool Pressed(ButtonType key) const;
		NODISC bool Released(ButtonType key) const;

	public:
		void Update(const Time& time, bool focus) override;
		void HandleEvent(const sf::Event& event) override;

	private:
		bool	m_current_state		[sf::Keyboard::KeyCount] = {false};
		bool	m_previous_state	[sf::Keyboard::KeyCount] = {false};
		float	m_held_time			[sf::Keyboard::KeyCount] = {0.0f};
	};
}