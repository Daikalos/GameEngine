#pragma once

#include <SFML/Graphics.hpp>

#include "Camera.h"

#include "../utilities/NonCopyable.h"

namespace fge
{
	enum class WindowBorder
	{
		Windowed,
		Fullscreen,
		BorderlessWindowed
	};

	// allow for toggle fullscreen, change resolution, and other settings
	//
	class Window : public sf::RenderWindow, NonCopyable
	{
	public:
		Window(std::string name, sf::VideoMode mode, WindowBorder window_border, sf::ContextSettings settings, bool vertical_sync, int frame_rate, Camera& camera);
		~Window();

		void initialize();
		void handle_event(const sf::Event& event);

		void set_framerate(int frame_rate);
		void set_vertical_sync(bool flag);

		void set_resolution(int index);

		void build(WindowBorder window_border, sf::VideoMode mode, sf::ContextSettings settings);

		void set_border(WindowBorder window_border);
		void set_mode(sf::VideoMode mode);
		void set_settings(sf::ContextSettings settings);

		// false = hides and grabs the cursor
		// true = shows and unhooks the cursor
		//
		void set_cursor_state(bool flag);

	public:
		std::vector<sf::VideoMode> get_modes() const;

	private:
		std::string _name;
		sf::VideoMode _mode;
		WindowBorder _window_border;
		sf::ContextSettings _settings;
		bool _vertical_sync;
		int _frame_rate;

		Camera* _camera;
	};
}