#pragma once

#include <SFML/Graphics.hpp>

#include "../utilities/NonCopyable.h"

namespace fge
{
	// allow for toggle fullscreen, change resolution, and other settings
	//
	class Window : public sf::RenderWindow, NonCopyable
	{
	public:
		Window();
		~Window();

		void onCreate() override;
		void onResize() override;

		bool initialize();
		void handle_event(const sf::Event& event);

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