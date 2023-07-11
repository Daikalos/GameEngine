#pragma once

#include <vector>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>

#include <Velox/Graphics/Batchable.h>

#include <Velox/Config.hpp>

namespace vlx::ui
{
	class VELOX_API TextMesh final
	{
	private:
		using VertexArray = std::vector<sf::Vertex>;

	public:
		NODISC constexpr sf::PrimitiveType GetPrimitive() const noexcept;

	public:
		//void Batch(SpriteBatch& sprite_batch, const Mat4f& transform, float depth) const override;

	private:
		mutable VertexArray	m_vertices;
		mutable VertexArray	m_outline;
	};

	constexpr sf::PrimitiveType	TextMesh::GetPrimitive() const noexcept
	{
		return sf::PrimitiveType::TriangleStrip;
	}
}