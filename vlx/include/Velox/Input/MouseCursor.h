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
		MouseCursor(Window& window, const sf::Texture& texture, bool locked);
		MouseCursor(Window& window, const sf::Texture& texture, float sensitivity);
		MouseCursor(Window& window, const sf::Texture& texture, float sensitivity, bool locked);

	public:
		[[nodiscard]] const sf::Vector2i& GetPosition() const noexcept;
		[[nodiscard]] const sf::Vector2f& GetDelta() const noexcept;
		[[nodiscard]] const float GetSensitivity() const noexcept;
		[[nodiscard]] const bool GetIsLocked() const noexcept;

		/// <summary>
		///		Sets the texture of the cursor
		/// </summary>
		void SetTexture(const sf::Texture& texture);

		/// <summary>
		///		Sets the sensitivity of the cursor, do note that it will not change the actual speed of the cursor,
		///		but rather increase the delta value returned, mostly useful for when cursor is locked
		/// </summary>
		void SetSensitivity(const float val) noexcept;

		/// <summary>
		///		Shows/Hides cursor
		/// </summary>
		void SetVisibility(const bool flag) noexcept;

		/// <summary>
		///		Locks the cursor to the middle of the window
		/// </summary>
		void SetIsLocked(const bool flag) noexcept;

	public:
		void Update(const Time& time, const bool focus) override;
		void HandleEvent(const sf::Event& event) override;

	private:
		Window*					m_window		{nullptr};
		sf::Cursor				m_cursor;

		sf::Vector2i			m_position;
		sf::Vector2f			m_delta;

		float					m_sensitivity	{1.0f};
		bool					m_locked		{true}; // locks the cursor inside the window if true
	};
}