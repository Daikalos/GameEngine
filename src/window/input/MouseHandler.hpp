#pragma once

#include <SFML/Window/Cursor.hpp>
#include <unordered_map>
#include <FGE/Concepts.hpp>

#include "InputHandler.hpp"

#include "../Camera.h"
#include "../Window.h"

#include <iostream>

namespace fge
{
	////////////////////////////////////////////////////////////
	// Handles all of the mouse input, only supports one mouse
	// at a time. 
	////////////////////////////////////////////////////////////
	class MouseHandler : public InputHandler
	{
	public:
		void Update(const Time& time, bool focus) override
		{
			m_scroll_delta = 0.0f;

			for (uint32_t i = 0; i < sf::Mouse::ButtonCount; ++i)
			{
				m_previous_button_state[i] = m_current_button_state[i];
				m_current_button_state[i] = focus && m_enabled && sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(i));

				m_button_held_timer[i] = m_current_button_state[i] ?
					m_button_held_timer[i] + (m_button_held_timer[i] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
			}
		}
		void HandleEvent(const sf::Event& event) override
		{
			switch (event.type)
			{
			case sf::Event::MouseWheelScrolled:
				m_scroll_delta = event.mouseWheelScroll.delta;
				break;
			}
		}

	public:
		bool ScrollUp() const noexcept { return m_scroll_delta > m_scroll_threshold; }
		bool ScrollDown() const noexcept { return m_scroll_delta < -m_scroll_threshold; }

		bool Held(const sf::Mouse::Button& button) const
		{
			return m_current_button_state[button] && m_button_held_timer[button] >= m_held_threshold;
		}
		bool Pressed(const sf::Mouse::Button& button) const
		{
			return m_current_button_state[button] && !m_previous_button_state[button];
		}
		bool Released(const sf::Mouse::Button& button) const
		{
			return !Pressed(button);
		}

	private:
		float	m_scroll_delta		{0.0f};
		float	m_scroll_threshold	{0.01f}; // threshold before scroll is considered for up/down

		bool	m_current_button_state	[sf::Mouse::ButtonCount] = {false};
		bool	m_previous_button_state	[sf::Mouse::ButtonCount] = {false};
		float	m_button_held_timer		[sf::Mouse::ButtonCount] = {0.0f};
	};

	template<Enum B>
	class MouseHandlerBindable : public MouseHandler
	{
	public:
		using MouseHandler::Held;
		using MouseHandler::Pressed;
		using MouseHandler::Released;

		bool Held(const B& button) const
		{
			return Held(m_button_bindings.at(button));
		}
		bool Pressed(const B& button) const
		{
			return Pressed(m_button_bindings.at(button));
		}
		bool Released(const B& button) const
		{
			return Released(m_button_bindings.at(button));
		}

		void SetBinding(const B& name, const sf::Mouse::Button& button)
		{
			m_button_bindings[name] = button;
		}
		void RemoveBinding(const B& name)
		{
			m_button_bindings.erase(name);
		}

	private:
		std::unordered_map<B, sf::Mouse::Button> m_button_bindings;	// bindings for buttons
	};
}