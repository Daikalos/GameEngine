#include <Velox/UI/Systems/TextSystem.h>

using namespace vlx::ui;

TextSystem::TextSystem(EntityAdmin& entity_admin, LayerType layer)
	: SystemAction(entity_admin, layer)
{

}

void TextSystem::Update()
{
}

void TextSystem::AddLine(
	std::vector<sf::Vertex>& m_vertices, 
	float line_length, 
	float line_top, 
	sf::Color color, 
	float offset, 
	float thickness, 
	float outline_thickness)
{
}

void TextSystem::AddGlyphQuad(
	std::vector<sf::Vertex>& m_vertices, 
	sf::Color color, 
	const sf::Glyph& glyph, 
	float italic_shear)
{

}
