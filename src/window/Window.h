#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>

namespace fge
{
	// allow for toggle fullscreen, change resolution, and other settings
	//
	class Window : public sf::RenderWindow, sf::NonCopyable
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