#pragma once

#include <SFML/Graphics.hpp>

#include "InputHandler.hpp"

#include "../graphics/ResourceHolder.hpp"
#include "../graphics/Resources.h"
#include "../window/Window.h"
#include "../Utilities.hpp"

namespace vlx
{
	namespace cst
	{
		enum class ID : uint32_t
		{
			None,
			Idle,
			Click,
			Loading,
			Grab
		};
	}

	class MouseCursor : public InputHandler
	{
	public:
		MouseCursor(Window& window, const TextureHolder& texture_holder)
			: m_window(&window), m_texture_holder(&texture_holder)
		{ 
			m_cursor_pos = sf::Vector2f(m_window->GetOrigin());
			sf::Mouse::setPosition(m_window->GetOrigin(), *m_window);

			SetCursor(m_texture_holder->Get(Texture::ID::IdleCursor));
		}

		void Update(const Time& time, bool focus) override
		{
			if (m_enabled && focus)
			{
				m_mouse_pos = sf::Mouse::getPosition(*m_window);
				m_mouse_delta = sf::Vector2f(m_mouse_pos - m_window->GetOrigin());
				sf::Mouse::setPosition(m_window->GetOrigin(), *m_window); // TODO: acts buggy if window origin is outside screen, maybe fix in future

				m_cursor_pos += m_mouse_delta * m_window->GetRatioCmp() * m_mouse_sensitivity;

				if (m_locked)
				{
					if (m_cursor_pos.x < 0.0f)
						m_cursor_pos.x = 0.0f;
					if (m_cursor_pos.y < 0.0f)
						m_cursor_pos.y = 0.0f;
					if (m_cursor_pos.x > m_window->getSize().x)
						m_cursor_pos.x = m_window->getSize().x;
					if (m_cursor_pos.y > m_window->getSize().y)
						m_cursor_pos.y = m_window->getSize().y;
				}

				m_cursor.setPosition(m_cursor_pos);
			}
		}
		void HandleEvent(const sf::Event& event)
		{
			switch (event.type)
			{
			case sf::Event::GainedFocus: 
				m_cursor_pos = sf::Vector2f(sf::Mouse::getPosition(*m_window));
				m_cursor.setPosition(m_cursor_pos);

				sf::Mouse::setPosition(m_window->GetOrigin(), *m_window); // set cursor to middle of screen to prevent inaccurate delta values
				break;
			case sf::Event::LostFocus:
				sf::Mouse::setPosition(sf::Vector2i(m_cursor_pos), *m_window);
				break;
			}
		}
		void Draw()
		{
			if (m_cursor_visible)
				m_window->draw(m_cursor);
		}

	public:
		[[nodiscard]] constexpr sf::Vector2i GetMousePosition() const noexcept { return m_mouse_pos; }
		[[nodiscard]] constexpr sf::Vector2f GetDelta() const noexcept { return m_mouse_delta; }
		[[nodiscard]] constexpr sf::Vector2f GetPosition() const noexcept { return m_cursor_pos; }

		void SetCursor(const sf::Texture& texture)
		{
			m_cursor.setTexture(texture);
		}
		void SetCursorState(cst::ID cursor_state)
		{
			m_cursor_state = cursor_state;
		}
		void SetMouseSensitivity(float val)
		{
			m_mouse_sensitivity = val;
		}
		void SetCursorVisible(bool flag)
		{
			m_cursor_visible = flag;
		}
		void SetCursorLocked(bool flag)
		{
			m_locked = flag;
		}

	private:
		Window*					m_window			{nullptr};
		const TextureHolder*	m_texture_holder	{nullptr};

		sf::Sprite				m_cursor;
		cst::ID					m_cursor_state		{cst::ID::Idle};

		sf::Vector2i			m_mouse_pos;
		sf::Vector2f			m_mouse_delta;
		sf::Vector2f			m_cursor_pos;

		float					m_mouse_sensitivity	{1.0f};

		bool					m_cursor_visible	{true};
		bool					m_locked			{true}; // locks the cursor inside the window if true
	};
}