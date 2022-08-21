#pragma once

#include <SFML/Graphics.hpp>

#include "../utilities/NonCopyable.h"

namespace fge
{
	////////////////////////////////////////////////////////////
	// settings for type of border
	////////////////////////////////////////////////////////////
	enum class WindowBorder
	{
		Windowed,
		Fullscreen,
		BorderlessWindowed // TODO : FIX BORDERLESS WINDOWED ON LOWER RESOLUTION, ALSO LOOK FOR SOLUTION ON FULLSCREEN AT MAX RESOLUTION
	};

	////////////////////////////////////////////////////////////
	// Expanded to allow for toggle fullscreen, 
	// change resolution, and other settings
	////////////////////////////////////////////////////////////
	class Window : public sf::RenderWindow, NonCopyable
	{
	public:
		Window(
			std::string& name, 
			const sf::VideoMode& mode, 
			const WindowBorder& window_border, 
			const sf::ContextSettings& settings, 
			bool vertical_sync, int frame_rate);

		void Initialize();
		void HandleEvent(const sf::Event& event);

		void SetFramerate(int frame_rate);
		void SetVerticalSync(bool flag);

		void SetResolution(int index);

		void Build(WindowBorder window_border, sf::VideoMode mode, sf::ContextSettings settings);

		void SetBorder(WindowBorder border);
		void SetMode(sf::VideoMode mode);
		void SetSettings(sf::ContextSettings settings);

		////////////////////////////////////////////////////////////
		// true = shows and unhooks the cursor
		// false = hides and grabs the cursor
		////////////////////////////////////////////////////////////
		void SetCursorState(bool flag);

	public:

		////////////////////////////////////////////////////////////
		// only gets modes that match the aspect ratio of 
		// the desktop
		////////////////////////////////////////////////////////////
		std::vector<sf::VideoMode> GetModes() const;

	private:
		std::string			m_name;				// name of window
		sf::VideoMode		m_mode;				// current mode set
		WindowBorder		m_border;			// current border type
		sf::ContextSettings m_settings;			// settings
		bool				m_vertical_sync;	// vertical sync
		int					m_frame_rate;		// maximum frame rate
	};
}