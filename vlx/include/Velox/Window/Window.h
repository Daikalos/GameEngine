#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include <string_view>

namespace vlx
{
	//////////////////////////////////////////////
	// 
	// settings for type of border
	// 
	//////////////////////////////////////////////
	enum class WindowBorder
	{
		Windowed,
		Fullscreen,
		BorderlessWindowed // TODO : FIX BORDERLESS WINDOWED ON LOWER RESOLUTION, ALSO LOOK FOR SOLUTION ON FULLSCREEN AT MAX RESOLUTION
	};

	//////////////////////////////////////////////
	// 
	// Expanded to allow for toggle fullscreen, 
	// change resolution, and other settings
	// 
	//////////////////////////////////////////////
	class VELOX_API Window final : public sf::RenderWindow, NonCopyable
	{
	public:
		Window(
			std::string_view name,
			const sf::VideoMode& mode, 
			const WindowBorder& window_border, 
			const sf::ContextSettings& settings, 
			bool vertical_sync, int frame_rate);

	public:
		//////////////////////////////////////////////
		// 
		// only gets modes that match the aspect ratio of the desktop
		// 
		//////////////////////////////////////////////
		[[nodiscard]] const std::vector<sf::VideoMode>& GetValidModes(bool update = false) const;

		[[nodiscard]] constexpr sf::Vector2i GetOrigin() const noexcept;
		[[nodiscard]] const sf::Vector2f& GetRatioCmp() const noexcept;

		//////////////////////////////////////////////
		// 
		// true = shows and unhooks the cursor
		// false = hides and grabs the cursor
		// 
		//////////////////////////////////////////////
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
		std::string			m_name;										// name of window
		sf::VideoMode		m_mode;										// current mode set
		WindowBorder		m_border		{WindowBorder::Windowed};	// current border type
		sf::ContextSettings m_settings;									// settings
		sf::Vector2f		m_ratio_cmp;								// current ratio in percentage when compared to default desktop mode
		bool				m_vertical_sync	{false};					// vertical sync
		int					m_frame_rate	{144};						// maximum frame rate
		bool				m_hide_cursor	{false};

		mutable std::vector<sf::VideoMode> m_cached_modes;
	};
}