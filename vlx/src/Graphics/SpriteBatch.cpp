#include <Velox/Graphics/SpriteBatch.h>

using namespace vlx;

SpriteBatch::SpriteBatch()
	: m_triangles()
{
	m_triangles.clear();
}

SpriteBatch::Triangle::Triangle(sf::Vertex&& v0, sf::Vertex&& v1, sf::Vertex&& v2, const sf::Texture* t, const sf::Shader* s, const float d)
	: texture(t), shader(s), depth(d)
{
	vertices[0] = v0;
	vertices[1] = v1;
	vertices[2] = v2;
}

void SpriteBatch::SetSortMode(const SortMode sort_mode)
{
	m_sort_mode = sort_mode;
	m_update_required = true;
}

void SpriteBatch::AddTriangle(const Transform& transform, const sf::Vertex& v0, const sf::Vertex& v1, const sf::Vertex& v2, const sf::Texture* texture, const sf::Shader* shader, float depth)
{
	m_triangles.emplace_back(
		sf::Vertex(transform.getTransform() * v0.position, v0.color, v0.texCoords),
		sf::Vertex(transform.getTransform() * v1.position, v1.color, v1.texCoords),
		sf::Vertex(transform.getTransform() * v2.position, v2.color, v2.texCoords), texture, shader, depth);

	m_update_required = true;
}

void SpriteBatch::Batch(const IBatchable& batchable, const Transform& transform, float depth)
{
	batchable.Batch(*this, transform, depth);
}

void SpriteBatch::Batch(const Transform& transform, const sf::VertexArray& vertices, sf::PrimitiveType type, const sf::Texture* texture, const sf::Shader* shader, float depth)
{
	const std::size_t count = vertices.getVertexCount();

	if (!count)
		return;

	switch (type)
	{
	case sf::Triangles:
		for (std::size_t i = 2; i < count; i += 3)
			AddTriangle(transform, vertices[i - 2], vertices[i - 1], vertices[i], texture, shader, depth);
		break;
	case sf::TriangleStrip:
		for (std::size_t i = 2; i < count; ++i)
			AddTriangle(transform, vertices[i - 2], vertices[i - 1], vertices[i], texture, shader, depth);
		break;
	case sf::TriangleFan:
		for (std::size_t i = 2; i < count; ++i)
			AddTriangle(transform, vertices[0], vertices[i - 1], vertices[i], texture, shader, depth);
		break;
	default:
		throw std::runtime_error("this primitive is not supported");
	}
}

void SpriteBatch::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
{
	if (m_update_required)
	{
		SortTriangles();
		CreateBatches();

		m_update_required = false;
	}

	sf::RenderStates states_copy(states);
	for (std::size_t i = 0, start = 0; i < m_batches.size(); ++i)
	{
		states_copy.texture = m_batches[i].texture;
		states_copy.shader = m_batches[i].shader;

		target.draw(&m_vertices[start], m_batches[i].count, sf::Triangles, states_copy);

		start += m_batches[i].count;
	}
}

void SpriteBatch::SortTriangles() const
{
	m_proxy.resize(m_triangles.size());
	for (std::size_t i = 0; i < m_proxy.size(); ++i)
		m_proxy[i] = &m_triangles[i];

	switch (m_sort_mode)
	{
	case SortMode::BackToFront:
		std::stable_sort(m_proxy.begin(), m_proxy.end(),
			[this](const Triangle* lhs, const Triangle* rhs)
			{
				return (lhs->depth != rhs->depth) ? 
					(lhs->depth > rhs->depth) : (lhs->texture < rhs->texture);
			});
		break;
	case SortMode::FrontToBack:
		std::stable_sort(m_proxy.begin(), m_proxy.end(),
			[this](const Triangle* lhs, const Triangle* rhs)
			{
				return (lhs->depth != rhs->depth) ?
					(lhs->depth < rhs->depth) : (lhs->texture < rhs->texture);
			});
		break;
	case SortMode::Texture:
		std::stable_sort(m_proxy.begin(), m_proxy.end(), 
			[this](const Triangle* lhs, const Triangle* rhs)
			{
				return lhs->texture < rhs->texture;
			});
		break;
	case SortMode::Deferred:
	default: break; // nothing to do
	}
}

void SpriteBatch::CreateBatches() const
{
	if (m_proxy.empty())
		return;

	m_vertices.resize(m_proxy.size() * TRIANGLE_COUNT);

	std::size_t start = 0, next = 0;
	const sf::Texture* last_texture = m_proxy.front()->texture;

	for (; next < m_proxy.size(); ++next)
	{
		auto next_texture = m_proxy[next]->texture;
		if (next_texture != last_texture)
		{
			m_batches.emplace_back(last_texture, m_proxy[next]->shader, (next - start) * TRIANGLE_COUNT);
			last_texture = next_texture;
			start = next;
		}

		for (std::size_t i = 0; i < TRIANGLE_COUNT; ++i)
			m_vertices[next * TRIANGLE_COUNT + i] = m_proxy[next]->vertices[i];
	}

	if (start != m_triangles.size())
		m_batches.emplace_back(m_proxy[start]->texture, m_proxy[start]->shader, (m_triangles.size() - start) * TRIANGLE_COUNT);
}

void SpriteBatch::Clear()
{
	m_triangles.clear();
	m_batches.clear();
	m_vertices.clear();

	m_update_required = false;
}