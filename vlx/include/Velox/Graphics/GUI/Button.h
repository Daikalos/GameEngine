#pragma once

#include <SFML/Graphics.hpp>

#include <functional>

namespace vlx::gui
{
	class Button
	{
	public:
		Button() = default;
		Button(sf::Vector2u size);

	public:
		template<typename Func>
		void OnPress(Func&& func);

		template<typename Func>
		void OnRelease(Func&& func);

		template<typename Func>
		void OnHover(Func&& func);

	private:
		sf::Vector2u			m_size;

		std::function<void()>	m_press_event;
		std::function<void()>	m_release_event;
		std::function<void()>	m_hover_event;
	};

	template<typename Func>
	inline void Button::OnPress(Func&& func)
	{
		m_press_event = std::forward<Func>(func);
	}

	template<typename Func>
	inline void Button::OnRelease(Func&& func)
	{
		m_release_event = std::forward<Func>(func);
	}

	template<typename Func>
	inline void Button::OnHover(Func&& func)
	{
		m_hover_event = std::forward<Func>(func);
	}
}