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

		bool initialize();

	public:
		std::vector<sf::VideoMode> get_modes() const
		{
			return _modes;
		}

	private:
		std::vector<sf::VideoMode> _modes;
		sf::ContextSettings _settings;
		bool _fullscreen;
	};
}