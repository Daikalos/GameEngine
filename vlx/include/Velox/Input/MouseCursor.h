#pragma once

#include <SFML/Window/Mouse.hpp>

#include <Velox/System/Vector2.hpp>
#include <Velox/Window/Window.h>
#include <Velox/Config.hpp>

#include "InputHandler.h"

namespace vlx
{
	class VELOX_API MouseCursor : public InputHandler
	{
	public:
		MouseCursor(Window& window);
		MouseCursor(Window& window, bool locked);
		MouseCursor(Window& window, float sensitivity);
		MouseCursor(Window& window, float sensitivity, bool locked);

	public:
		NODISC const Vector2i& GetPosition() const noexcept;
		NODISC const Vector2f& GetDelta() const noexcept;
		NODISC float GetSensitivity() const noexcept;
		NODISC bool GetIsLocked() const noexcept;

		///	Determines the texture of the cursor when window is in focus.
		/// 
		/// \param Texture: texture to use
		/// 
		void SetTexture(const sf::Texture& texture);

		///	Determines the sensitivity of the cursor, do note that it will not change the actual speed of the cursor,
		///	but rather increase the delta value returned, mostly useful for when cursor is locked.
		/// 
		/// \param Sensitivity: value of sensitivity
		/// 
		void SetSensitivity(float sensitivity) noexcept;

		/// Show or hide cursor.
		/// 
		/// \param Flag: true to show, and false to hide
		/// 
		void SetVisibility(bool flag) noexcept;

		///	Locks the cursor to the middle of the window
		/// 
		/// \param Flag: true to lock, and false to unlock (move freely)
		/// 
		void SetIsLocked(bool flag) noexcept;

	public:
		void Update(const Time& time, bool focus) override;
		void HandleEvent(const sf::Event& event) override;

	private:
		Window*					m_window		{nullptr};

		sf::Cursor				m_cursor;
		const sf::Texture*		m_texture		{nullptr};

		Vector2i				m_position;
		Vector2f				m_delta;

		float					m_sensitivity	{1.0f};
		bool					m_locked		{false}; // locks the cursor inside the window if true
		bool					m_in_focus		{true};
	};
}