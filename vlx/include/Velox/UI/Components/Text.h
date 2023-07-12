#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

namespace vlx::ui
{
	///	Thanks to SFML for a similar implementation, sf::Text was not used because of 
	/// the redundant data which would conflict with the ECS
	/// 
	class VELOX_API Text
	{
	public:
		enum Style : uint8
		{
			Regular			= 0,
			Bold			= 1 << 0,
			Italic			= 1 << 1,
			Underlined		= 1 << 2,
			StrikeThrough	= 1 << 3
		};

	public:
		const std::string&	GetString() const noexcept;
		const sf::Font*		GetFont() const noexcept;
		const uint32		GetCharacterSize() const noexcept;
		float				GetLetterSpacing() const noexcept;
		float				GetLineSpacing() const noexcept;
		float				GetOutlineThickness() const noexcept;
		sf::Color			GetFillColor() const noexcept;
		sf::Color			GetOutlineColor() const noexcept;
		uint8				GetStyle() const noexcept;

		void SetString(				const std::string& text);
		void SetFont(				const sf::Font* font);
		void SetCharacterSize(		uint32 character_size);
		void SetLetterSpacing(		float spacing_factor);
		void SetLineSpacing(		float spacing_factor);
		void SetOutlineThickness(	float thickness);
		void SetFillColor(			sf::Color color);
		void SetOutlineColor(		sf::Color color);
		void SetStyle(				uint8 style);

	private:
		std::string		m_text;
		const sf::Font*	m_font					{nullptr};
		uint32			m_character_size		{30};
		float			m_letter_spacing_factor {1.0f};
		float			m_line_spacing_factor	{1.0f};
		float			m_outline_thickness		{0.0f};
		sf::Color		m_fill_color			{sf::Color::White};
		sf::Color		m_outline_color			{sf::Color::Black};
		uint64			m_texture_id			{0};
		uint8			m_style					{Regular};
		bool			m_update_mesh			{true};
		bool			m_update_fill			{true};
		bool			m_update_outline		{true};

		friend class TextSystem;
	};
}

