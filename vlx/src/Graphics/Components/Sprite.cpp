#include <Velox/Graphics/Components/Sprite.h>

#include <Velox/Graphics/SpriteBatch.h>
#include <Velox/System/Mat4f.hpp>

using namespace vlx;

Sprite::Sprite(const sf::Texture& texture, float depth) : m_depth(depth)
{
	SetTexture(texture, true);
}

Sprite::Sprite(const sf::Texture& texture, const Vector2f& size, float depth) : m_depth(depth)
{
	SetSize(size);
	SetTexture(texture, true);
}

Sprite::Sprite(const sf::Texture& texture, const RectFloat& visible_rect, float depth) : m_depth(depth)
{
	SetTextureRect(visible_rect);
	SetTexture(texture);
}

Sprite::Sprite(const sf::Texture& texture, const Vector2f& size, const RectFloat& visible_rect, float depth) : m_depth(depth)
{
	SetTextureRect(visible_rect);
	SetSize(size);
	SetTexture(texture);
}

const sf::Texture* Sprite::GetTexture() const noexcept
{
	return m_texture;
}
const sf::Shader* Sprite::GetShader() const noexcept
{
	return m_shader;
}
const Sprite::VertexArray& Sprite::GetVertices() const noexcept
{
	return m_vertices;
}
RectFloat Sprite::GetTextureRect() const noexcept
{
	return RectFloat(m_vertices.front().texCoords, m_vertices.back().texCoords);
}
Vector2f Sprite::GetSize() const noexcept
{
	return m_vertices.back().position;
}
float Sprite::GetDepth() const noexcept
{
	return m_depth;
}
float Sprite::GetOpacity() const noexcept
{
	return (float)m_vertices[0].color.a / 255.0f;
}

void Sprite::SetTexture(const sf::Texture& texture, bool reset_rect, bool reset_size)
{
	if (reset_rect || (m_texture == nullptr && GetTextureRect() == RectFloat()))
		SetTextureRect(RectFloat({ 0.0f, 0.0f }, Vector2f(texture.getSize())));

	if (reset_size || (m_texture == nullptr && GetSize() == Vector2f::Zero))
		SetSize(Vector2f(texture.getSize()));

	m_texture = &texture;
}
void Sprite::SetTextureRect(const RectFloat& rect)
{
	if (GetTextureRect() != rect)
		UpdateTexCoords(rect);
}
void Sprite::SetSize(const Vector2f& size)
{
	if (GetSize() != size)
		UpdatePositions(size);
}
void Sprite::SetColor(const sf::Color& color)
{
	for (sf::Vertex& vertex : m_vertices) // do not change alpha
	{
		vertex.color.r = color.r;
		vertex.color.g = color.g;
		vertex.color.b = color.b;
	}
}
void Sprite::SetOpacity(float opacity)
{
	const uint8 alpha = static_cast<uint8>(opacity * UINT8_MAX);
	for (sf::Vertex& vertex : m_vertices)
		vertex.color.a = alpha;
}
void Sprite::SetDepth(float val)
{
	m_depth = val;
}

void Sprite::UpdatePositions(const Vector2f& size)
{
	m_vertices[0].position = Vector2f(0.0f,		0.0f);
	m_vertices[1].position = Vector2f(0.0f,		size.y);
	m_vertices[2].position = Vector2f(size.x,	0.0f);
	m_vertices[3].position = Vector2f(size.x,	size.y);
}
void Sprite::UpdateTexCoords(const RectFloat& texture_rect)
{
	float left		= texture_rect.left;
	float right		= left + texture_rect.width;
	float top		= texture_rect.top;
	float bottom	= top + texture_rect.height;

	m_vertices[0].texCoords = Vector2f(left,	top);
	m_vertices[1].texCoords = Vector2f(left,	bottom);
	m_vertices[2].texCoords = Vector2f(right,	top);
	m_vertices[3].texCoords = Vector2f(right,	bottom);
}

void Sprite::BatchImpl(SpriteBatch& sprite_batch, const Mat4f& transform, float depth) const
{
	sprite_batch.Batch(transform, m_vertices, GetPrimitive(), m_texture, m_shader, m_depth);
}