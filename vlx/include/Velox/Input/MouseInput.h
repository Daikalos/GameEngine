#pragma once

#include <SFML/Window/Cursor.hpp>

#include <Velox/Config.hpp>

#include "InputHandler.h"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Handles all of the mouse input, only supports one mouse
	// at a time. 
	////////////////////////////////////////////////////////////
	class VELOX_API MouseInput : public InputHandler
	{
	public:
		void Update(const Time& time, const bool focus) override;
		void HandleEvent(const sf::Event& event) override;

	public:
		constexpr bool ScrollUp() const noexcept;
		constexpr bool ScrollDown() const noexcept;

		bool Held(const sf::Mouse::Button button) const;
		bool Pressed(const sf::Mouse::Button button) const;
		bool Released(const sf::Mouse::Button button) const;

	private:
		float	m_scroll_delta			{0.0f};
		float	m_scroll_threshold		{0.01f}; // threshold before scroll is considered up/down

		bool	m_current_button_state	[sf::Mouse::ButtonCount] = {false};
		bool	m_previous_button_state	[sf::Mouse::ButtonCount] = {false};
		float	m_held_timer			[sf::Mouse::ButtonCount] = {0.0f};
	};
}