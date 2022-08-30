#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Utilities.hpp>

#include <string_view>

namespace vlx
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
	class Window final : public sf::RenderWindow, NonCopyable
	{
	public:
		Window(
			std::string_view name,
			const sf::VideoMode& mode, 
			const WindowBorder& window_border, 
			const sf::ContextSettings& settings, 
			bool vertical_sync, int frame_rate);

	public:
		////////////////////////////////////////////////////////////
		// only gets modes that match the aspect ratio of 
		// the desktop
		////////////////////////////////////////////////////////////
		[[nodiscard]] std::vector<sf::VideoMode> GetValidModes(bool update = true) const;

		[[nodiscard]] constexpr sf::Vector2i GetOrigin() const noexcept;
		[[nodiscard]] constexpr sf::Vector2f GetRatioCmp() const noexcept;

		////////////////////////////////////////////////////////////
		// true = shows and unhooks the cursor
		// false = hides and grabs the cursor
		////////////////////////////////////////////////////////////
		void SetCursorState(bool flag);

	public:
		void onCreate() override;

		void Initialize();
		void HandleEvent(const sf::Event& event);

		void SetFramerate(int frame_rate);
		void SetVerticalSync(bool flag);

		void SetResolution(int index);

		void Build(WindowBorder window_border, sf::VideoMode mode, sf::ContextSettings settings);

		void SetBorder(WindowBorder border);
		void SetMode(sf::VideoMode mode);
		void SetSettings(sf::ContextSettings settings);

	private:
		std::string			m_name;				// name of window
		sf::VideoMode		m_mode;				// current mode set
		WindowBorder		m_border;			// current border type
		sf::ContextSettings m_settings;			// settings
		sf::Vector2f		m_ratio_cmp;		// current ratio in percentage when compared to desktop
		bool				m_vertical_sync;	// vertical sync
		int					m_frame_rate;		// maximum frame rate

		std::vector<sf::VideoMode> m_cached_modes;
	};

	[[nodiscard]] inline constexpr sf::Vector2i Window::GetOrigin() const noexcept
	{
		return sf::Vector2i(getSize() / 2u);
	}
	[[nodiscard]] inline constexpr sf::Vector2f Window::GetRatioCmp() const noexcept
	{
		return m_ratio_cmp;
	}
}