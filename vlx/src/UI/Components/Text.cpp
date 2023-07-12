#include <Velox/UI/Components/Text.h>

using namespace vlx::ui;

const std::string& Text::GetString() const noexcept			{ return m_text; }
const sf::Font* Text::GetFont() const noexcept				{ return m_font; }
const vlx::uint32 Text::GetCharacterSize() const noexcept	{ return m_character_size; }
float Text::GetLetterSpacing() const noexcept				{ return m_letter_spacing_factor; }
float Text::GetLineSpacing() const noexcept					{ return m_line_spacing_factor; }
float Text::GetOutlineThickness() const noexcept			{ return m_outline_thickness; }
sf::Color Text::GetFillColor() const noexcept				{ return m_fill_color; }
sf::Color Text::GetOutlineColor() const noexcept			{ return m_outline_color; }
vlx::uint8 Text::GetStyle() const noexcept					{ return m_style; }

void Text::SetString(const std::string& text)
{
	if (m_text != text)
	{
		m_text = text;
		m_update_mesh = true;
	}
}

void Text::SetFont(const sf::Font* font)
{
	if (m_font != font)
	{
		m_font = font;
		m_update_mesh = true;
	}
}

void Text::SetCharacterSize(uint32 character_size)
{
	if (m_character_size != character_size)
	{
		m_character_size = character_size;
		m_update_mesh = true;
	}
}

void Text::SetLetterSpacing(float spacing_factor)
{
	if (m_letter_spacing_factor != spacing_factor)
	{
		m_letter_spacing_factor = spacing_factor;
		m_update_mesh = true;
	}
}

void Text::SetLineSpacing(float spacing_factor)
{
	if (m_line_spacing_factor != spacing_factor)
	{
		m_line_spacing_factor = spacing_factor;
		m_update_mesh = true;
	}
}

void Text::SetOutlineThickness(float thickness)
{
	if (m_outline_thickness != thickness)
	{
		m_outline_thickness = thickness;
		m_update_mesh = true;
	}
}

void Text::SetFillColor(sf::Color color)
{
	if (m_fill_color != color)
	{
		m_fill_color = color;
		m_update_fill = true;
	}
}

void Text::SetOutlineColor(sf::Color color)
{
	if (m_outline_color != color)
	{
		m_outline_color = color;
		m_update_outline = true;
	}
}

void Text::SetStyle(uint8 style)
{
	if (m_style != style)
	{
		m_style = style;
	}
}
