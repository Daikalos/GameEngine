#include <Velox/Components/Sprite.h>

using namespace vlx;

static constexpr std::size_t VERTEX_COUNT = 4;

Sprite::Sprite(const sf::Texture& texture, float depth)
	: m_texture(nullptr), m_vertices(GetPrimitive(), VERTEX_COUNT), m_texture_rect(), m_depth(depth)
{
	SetTexture(texture, true);
}

Sprite::Sprite(const sf::Texture& texture, const sf::IntRect& rect, float depth)
{
	SetTextureRect(rect);
	SetTexture(texture);
}

void Sprite::Batch(SpriteBatch& sprite_batch, float depth) const
{
	sprite_batch.Batch(m_vertices, GetPrimitive(), m_texture, m_shader, )
}

const sf::Texture* Sprite::GetTexture() const noexcept
{
	return m_texture;
}
const sf::Shader* Sprite::GetShader() const noexcept
{
	return m_shader;
}
const sf::VertexArray& Sprite::GetVertices() const noexcept
{
	return m_vertices;
}
const sf::FloatRect Sprite::GetLocalBounds() const noexcept
{
	auto width = static_cast<float>(std::abs(m_texture_rect.width));
	auto height = static_cast<float>(std::abs(m_texture_rect.height));

	return sf::FloatRect({0.0f, 0.0f}, {width, height});
}
constexpr sf::PrimitiveType Sprite::GetPrimitive() const noexcept
{
	return sf::TriangleStrip;
}
constexpr float Sprite::GetDepth() const noexcept
{
	return m_depth;
}
void Sprite::SetTexture(const sf::Texture& texture, bool reset_rect)
{
	if (reset_rect || (!m_texture && (m_texture_rect == sf::IntRect())))
		SetTextureRect(sf::IntRect({ 0,0 }, { sf::Vector2i(texture.getSize()) }));

	m_texture = &texture;
}
void Sprite::SetTextureRect(const sf::IntRect& rect)
{
	if (m_texture_rect != rect)
	{
		m_texture_rect = rect;
		UpdatePositions();
		UpdateTexCoords();
	}
}
void Sprite::SetColor(const sf::Color& color)
{
	for (std::size_t i = 0; i < VERTEX_COUNT; ++i)
		m_vertices[i].color = color;
}
void Sprite::SetDepth(const float val) noexcept
{
	m_depth = val;
}

void Sprite::UpdatePositions()
{
	sf::FloatRect bounds = GetLocalBounds();

	m_vertices[0].position = sf::Vector2f(0.0f,			0.0f);
	m_vertices[1].position = sf::Vector2f(0.0f,			bounds.height);
	m_vertices[2].position = sf::Vector2f(bounds.width, 0.0f);
	m_vertices[3].position = sf::Vector2f(bounds.width, bounds.height);
}
void Sprite::UpdateTexCoords()
{
	const sf::FloatRect texture_rect(m_texture_rect);

	const float left	= texture_rect.left;
	const float right	= left + texture_rect.width;
	const float top		= texture_rect.top;
	const float bottom	= top + texture_rect.height;

	m_vertices[0].texCoords = sf::Vector2f(left,	top);
	m_vertices[1].texCoords = sf::Vector2f(left,	bottom);
	m_vertices[2].texCoords = sf::Vector2f(right,	top);
	m_vertices[3].texCoords = sf::Vector2f(right,	bottom);
}