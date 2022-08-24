#pragma once

#include <SFML/Window/Cursor.hpp>
#include <unordered_map>

#include "InputHandler.hpp"

#include "../Camera.h"
#include "../Window.h"

#include <iostream>

namespace fge
{
	template<class B, typename std::enable_if_t<std::is_enum_v<B>, bool> = true>
	class MouseHandler : public InputHandler
	{
	public:
		MouseHandler(const Window& window, const Camera& camera)
			: m_window(&window), m_camera(&camera)
		{
			m_cursor_pos = sf::Vector2f(m_window->GetOrigin());
			sf::Mouse::setPosition(m_window->GetOrigin(), *m_window);
		}

		void Update(const Time& time, bool focus) override
		{
			m_scroll_delta = 0.0f;

			if (focus)
			{
				m_mouse_pos = sf::Mouse::getPosition(*m_window);
				m_mouse_delta = m_mouse_pos - m_window->GetOrigin();
				sf::Mouse::setPosition(m_window->GetOrigin(), *m_window); // TODO: acts buggy if window origin is outside screen, maybe fix in future

				m_cursor_pos += sf::Vector2f(m_mouse_delta) * m_mouse_sensitivity;

				if (m_cursor_pos.x < 0.0f)
					m_cursor_pos.x = 0.0f;
				if (m_cursor_pos.y < 0.0f)
					m_cursor_pos.y = 0.0f;
				if (m_cursor_pos.x > m_window->getSize().x)
					m_cursor_pos.x = m_window->getSize().x;
				if (m_cursor_pos.y > m_window->getSize().y)
					m_cursor_pos.y = m_window->getSize().y;

				m_cursor.setPosition(m_cursor_pos);
			}

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
			case sf::Event::GainedFocus:
				sf::Mouse::setPosition(m_window->GetOrigin(), *m_window);
				break;
			}
		}
		void Draw(sf::RenderWindow& window)
		{
			if (m_cursor_visible)
				window.draw(m_cursor);
		}

	public:
		sf::Vector2i GetPosition() const
		{
			return m_mouse_pos;
		}
		sf::Vector2i GetDelta() const
		{
			return m_mouse_delta;
		}

		void SetCursor(const sf::Texture& texture)
		{
			m_cursor.setTexture(texture);
		}
		void SetCursorVisible(bool flag)
		{
			m_cursor_visible = flag;
		}
		void SetMouseSensitivity(float val)
		{
			m_mouse_sensitivity = val;
		}

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

		bool Held(const B& button) const
		{
			if (!m_button_bindings.contains(button))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(button)) + "] does not exist"); // note: only prints value and not name

			return Held(m_button_bindings.at(button));
		}
		bool Pressed(const B& button) const
		{
			if (!m_button_bindings.contains(button))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(button)) + "] does not exist");

			return Pressed(m_button_bindings.at(button));
		}
		bool Released(const B& button) const
		{
			if (!m_button_bindings.contains(button))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(button)) + "] does not exist");

			return Released(m_button_bindings.at(button));
		}

		void SetBinding(const B& name, const sf::Mouse::Button& button)
		{
			m_button_bindings[name] = button;
		}
		void RemoveBinding(const B& name)
		{
			if (!m_button_bindings.contains(name))
				throw std::runtime_error("The binding: [" + std::to_string(static_cast<uint32_t>(name)) + "] does not exist");

			m_button_bindings.erase(name);
		}

	private:
		const Window* m_window		{nullptr};
		const Camera* m_camera		{nullptr};

		sf::Sprite m_cursor;

		sf::Vector2i m_mouse_pos;
		sf::Vector2i m_mouse_delta;
		sf::Vector2f m_cursor_pos;

		bool m_cursor_visible		{true};

		float	m_scroll_delta		{0.0f};
		float	m_scroll_threshold	{0.01f}; // threshold before scroll is considered for up/down
		float	m_mouse_sensitivity	{1.0f};	

		bool	m_current_button_state	[sf::Mouse::ButtonCount] = {false};
		bool	m_previous_button_state	[sf::Mouse::ButtonCount] = {false};
		float	m_button_held_timer		[sf::Mouse::ButtonCount] = {0.0f};

		std::unordered_map<B, sf::Mouse::Button> m_button_bindings;	// bindings for buttons
	};
}