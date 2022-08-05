#pragma once

#include <SFML/Graphics.hpp>

namespace sfpl
{
	class Window : public sf::RenderWindow
	{
	public:
		Window();
		~Window();

		void refresh();

		bool set_resolution(int x, int y);
		void set_settings();
		bool initialize();

	public:
		std::vector<sf::VideoMode> get_modes() const
		{
			return modes;
		}
		sf::View get_view() const
		{
			return view;
		}

	private:
		std::vector<sf::VideoMode> modes;
		sf::ContextSettings settings;
		sf::View view;
	};
}