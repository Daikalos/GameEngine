#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Graphics/ResourceHolder.hpp>
#include <Velox/Graphics/Resources.h>
#include <Velox/Window/Window.h>
#include <Velox/Utilities.hpp>

#include "InputHandler.h"

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

	class VELOX_API MouseCursor : public InputHandler
	{
	public:
		MouseCursor(Window& window, const TextureHolder& texture_holder);

		void Update(const Time& time, const bool focus) override;
		void HandleEvent(const sf::Event& event) override;
		void Draw();

	public:
		[[nodiscard]] constexpr sf::Vector2i GetMousePosition() const noexcept;
		[[nodiscard]] constexpr sf::Vector2f GetDelta() const noexcept;
		[[nodiscard]] constexpr sf::Vector2f GetPosition() const noexcept;

		void SetCursor(const sf::Texture& texture);
		void SetCursorState(const cst::ID cursor_state);
		void SetMouseSensitivity(const float val) noexcept;
		void SetCursorVisible(const bool flag) noexcept;
		void SetCursorLocked(const bool flag) noexcept;

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