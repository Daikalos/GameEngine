#pragma once

#include <SFML/Window/Mouse.hpp>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "InputHandler.h"

namespace vlx
{
	/// Handles all of the mouse input, only supports one mouse at a time. 
	///
	class VELOX_API MouseInput final : public InputHandler
	{
	public:
		using ButtonType = sf::Mouse::Button;

	public:
		NODISC bool ScrollUp() const noexcept;
		NODISC bool ScrollDown() const noexcept;

		NODISC bool Held(ButtonType button) const;
		NODISC bool Pressed(ButtonType button) const;
		NODISC bool Released(ButtonType button) const;

	public:
		void Update(const Time& time, const bool focus) override;
		void HandleEvent(const sf::Event& event) override;

	private:
		float	m_scroll_delta		{0.0f};
		float	m_scroll_threshold	{0.01f}; // threshold before scroll is considered up/down

		bool	m_current_state		[sf::Mouse::ButtonCount] = {false};
		bool	m_previous_state	[sf::Mouse::ButtonCount] = {false};
		float	m_held_time			[sf::Mouse::ButtonCount] = {0.0f};
	};
}