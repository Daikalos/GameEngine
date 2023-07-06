#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

namespace vlx::ui
{
	///	Thanks to SFML for a similar implementation, sf::Text was not used because of 
	/// the redundant data which would conflict with the ECS
	/// 
	class VELOX_API Label
	{
	private:
		using VertexArray = std::vector<sf::Vertex>;

	public:
		enum Style
		{
			Regular			= 0,
			Bold			= 1 << 0,
			Italic			= 1 << 1,
			Underlined		= 1 << 2,
			StrikeThrough	= 1 << 3
		};

	public:


	private:
		std::string		m_text;
		const sf::Font*	m_font					{nullptr};
		uint32			m_character_size		{30};
		float			m_letter_spacing_factor {1.0f};
		float			m_line_spacing_factor	{1.0f};
		uint8			m_style					{Regular};
		sf::Color		m_fill_color			{sf::Color::White};
		sf::Color		m_outline_color			{sf::Color::Black};
		float			m_outline_thickness		{0.0f};

		mutable VertexArray		m_vertices;
		mutable VertexArray		m_outline;


	};

	constexpr int a = sizeof(Label);
}

