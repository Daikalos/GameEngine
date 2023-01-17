#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Graphics/ResourceHolder.hpp>
#include <Velox/Window/Window.h>
#include <Velox/Utilities.hpp>

#include "InputHandler.h"

namespace vlx
{
	namespace cst
	{
		enum class ID : std::uint8_t
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
		MouseCursor(Window& window, const sf::Texture& texture);

	public:
		[[nodiscard]] const sf::Vector2i& GetMousePosition() const noexcept;
		[[nodiscard]] const sf::Vector2f& GetDelta() const noexcept;
		[[nodiscard]] const sf::Vector2f& GetPosition() const noexcept;

		void SetTexture(const sf::Texture& texture);
		void SetState(const cst::ID cursor_state);
		void SetSensitivity(const float val) noexcept;
		void SetVisibility(const bool flag) noexcept;
		void SetIsLocked(const bool flag) noexcept;

	public:
		void Update(const Time& time, const bool focus) override;
		void HandleEvent(const sf::Event& event) override;

		void Draw();

	private:
		Window*					m_window			{nullptr};

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