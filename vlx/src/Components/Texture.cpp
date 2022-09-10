#include <Velox/Components/Texture.h>

using namespace vlx;

static constexpr std::size_t VERTEX_COUNT = 4;

Texture::Texture(const sf::Texture& texture, float depth)
	: m_texture(), m_vertices(sf::TriangleStrip, VERTEX_COUNT), m_texture_rect(), m_depth(depth)
{
	SetTexture(texture);
}

const sf::Texture* Texture::GetTexture() const noexcept
{
	return m_texture;
}
const sf::VertexArray& Texture::GetVertices() const noexcept
{
	return m_vertices;
}
constexpr const float Texture::GetDepth() const noexcept
{
	return m_depth;
}
void Texture::SetTexture(const sf::Texture& texture)
{

}
void Texture::SetTextureRect(const sf::IntRect& rect)
{
	if (m_texture_rect != rect)
	{
		m_texture_rect = rect;
		UpdatePositions();
		UpdateTexCoords();
	}
}
void Texture::SetColor(const sf::Color& color)
{
	for (std::size_t i = 0; i < VERTEX_COUNT; ++i)
		m_vertices[i].color = color;
}
void Texture::SetDepth(const float val) noexcept
{
	m_depth = val;
}

void Texture::UpdatePositions()
{

}
void Texture::UpdateTexCoords()
{
	const sf::FloatRect convertedTextureRect(m_texture_rect);

	const float left	= convertedTextureRect.left;
	const float right	= left + convertedTextureRect.width;
	const float top		= convertedTextureRect.top;
	const float bottom	= top + convertedTextureRect.height;

	m_vertices[0].texCoords = sf::Vector2f(left, top);
	m_vertices[1].texCoords = sf::Vector2f(left, bottom);
	m_vertices[2].texCoords = sf::Vector2f(right, top);
	m_vertices[3].texCoords = sf::Vector2f(right, bottom);
}