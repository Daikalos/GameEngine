#include <Velox/Graphics/Components/Mesh.h>

#include <Velox/Graphics/SpriteBatch.h>

using namespace vlx;

Mesh::Mesh() = default;

Mesh::Mesh(const sf::Texture& texture, float depth)
    : m_texture(&texture), m_shader(nullptr), m_vertices(), m_depth(depth)
{

}

Mesh::Mesh(VertexSpan vertices, const sf::Texture& texture, float depth) 
    : m_texture(&texture), m_shader(nullptr), m_vertices(), m_depth(depth)
{
    Assign(vertices);
}

sf::Vertex& Mesh::operator[](std::size_t i)
{
    return m_vertices[i];
}

const sf::Vertex& Mesh::operator[](std::size_t i) const
{
    return m_vertices[i];
}

const sf::Texture* Mesh::GetTexture() const noexcept
{
    return m_texture;
}

const sf::Shader* Mesh::GetShader() const noexcept
{
    return m_shader;
}

auto Mesh::GetVertices() const noexcept -> const VertexList&
{
    return m_vertices;
}

float Mesh::GetDepth() const noexcept
{
    return m_depth;
}

float Mesh::GetOpacity() const noexcept
{
    if (m_vertices.empty())
        return 0.0f;

    return (float)m_vertices[0].color.a / 255.0f;
}

void Mesh::SetTexture(const sf::Texture& texture)
{
    m_texture = &texture;
}

void Mesh::SetColor(const sf::Color& color)
{
    for (sf::Vertex& vertex : m_vertices)
    {
        vertex.color.r = color.r;
        vertex.color.g = color.g;
        vertex.color.b = color.b;
    }
}

void Mesh::SetDepth(float depth)
{
    m_depth = depth;
}

void Mesh::SetOpacity(float opacity)
{
    const uint8 alpha = static_cast<uint8>(opacity * UINT8_MAX);
    for (sf::Vertex& vertex : m_vertices)
        vertex.color.a = alpha;
}

std::size_t Mesh::GetSize() const noexcept
{
    return m_vertices.size();
}

void Mesh::Reserve(std::size_t capacity)
{
    m_vertices.reserve(capacity);
    m_indices.reserve(capacity);
}

void Mesh::Assign(VertexSpan vertices)
{
    if (vertices.size() < 3)
        return;

    m_vertices.clear();
    m_indices.clear();

    for (std::size_t i = 2; i < vertices.size(); i += TRIANGLE_COUNT)
    {
        m_vertices.emplace_back(vertices[i - 2]);
        m_vertices.emplace_back(vertices[i - 1]);
        m_vertices.emplace_back(vertices[i]);

        m_indices.emplace_back(i - 2);
        m_indices.emplace_back(i - 1);
        m_indices.emplace_back(i);
    }
}

void Mesh::Assign(VertexSpan vertices, IndicesSpan indices)
{
    if (vertices.size() < TRIANGLE_COUNT || indices.size() < TRIANGLE_COUNT)
        return;

    m_vertices.assign(vertices.begin(), vertices.end());
    m_indices.assign(indices.begin(), indices.end());
}

void Mesh::Assign(std::span<const Vector2f> vertices, IndicesSpan indices)
{
    if (vertices.size() < TRIANGLE_COUNT || indices.size() < TRIANGLE_COUNT)
        return;

    m_vertices.clear();
    for (std::size_t i = 0; i < vertices.size(); ++i)
        m_vertices.emplace_back(vertices[i]);

    m_indices.assign(indices.begin(), indices.end());
}

void Mesh::Assign(std::span<const Vector2f> polygon)
{
    if (polygon.size() < 3)
        return;

    auto indices = py::Triangulate(polygon);

    if (indices.has_value())
    {
        for (std::size_t i = 0; i < polygon.size(); ++i)
            m_vertices.emplace_back(polygon[i]);

        m_indices = indices.value();
    }
}

void Mesh::Push(Triangle&& triangle)
{
    for (uint8 i = 0; i < TRIANGLE_COUNT; ++i)
        m_vertices.emplace_back(std::move(triangle[i]));
}

void Mesh::Push(const Triangle& triangle)
{
    for (uint8 i = 0; i < TRIANGLE_COUNT; ++i)
        m_vertices.emplace_back(triangle[i]);
}

void Mesh::Push(sf::Vertex&& v0, sf::Vertex&& v1, sf::Vertex&& v2)
{
    m_vertices.emplace_back(std::move(v0));
    m_vertices.emplace_back(std::move(v1));
    m_vertices.emplace_back(std::move(v2));
}

void Mesh::Push(const sf::Vertex& v0, const sf::Vertex& v1, const sf::Vertex& v2)
{
    m_vertices.emplace_back(v0);
    m_vertices.emplace_back(v1);
    m_vertices.emplace_back(v2);
}

bool Mesh::Remove(std::size_t i)
{
    std::size_t tri = i * TRIANGLE_COUNT;

    if (tri >= (m_vertices.size() / 3))
        return false;

    m_vertices.erase(m_vertices.begin() + tri, m_vertices.begin() + (tri + TRIANGLE_COUNT));

    return true;
}

void Mesh::BatchImpl(SpriteBatch& sprite_batch, const Mat4f& transform, float depth) const
{
    sprite_batch.Batch(transform, m_vertices, m_indices, m_texture, m_shader, m_depth);
}
