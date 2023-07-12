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

}

void Text::SetFont(const sf::Font* font)
{
}

void Text::SetCharacterSize(uint32 character_size)
{
}

void Text::SetLetterSpacing(float spacing_factor)
{
}

void Text::SetLineSpacing(float spacing_factor)
{
}

void Text::SetOutlineThickness(float thickness)
{
}

void Text::SetFillColor(const sf::Color& color)
{
}

void Text::SetOutlineColor(const sf::Color& color)
{
}

void Text::SetStyle(uint8 style)
{
}
