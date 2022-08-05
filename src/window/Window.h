#pragma once

#include <SFML/Graphics.hpp>

namespace fge
{
	class Window : public sf::RenderWindow
	{
	public:
		Window();
		~Window();

		void onCreate() override;
		void onResize() override;

		void refresh();

		bool set_resolution(int x, int y);
		void set_settings();
		bool initialize();

	public:
		std::vector<sf::VideoMode> get_modes() const
		{
			return _modes;
		}
		sf::View get_view() const
		{
			return _view;
		}

	private:
		sf::View _view;

		std::vector<sf::VideoMode> _modes;
		sf::ContextSettings _settings;
		bool _fullscreen;
	};
}