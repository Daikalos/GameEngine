#pragma once

#include <vector>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>

#include <Velox/Graphics/Batchable.h>

#include <Velox/Config.hpp>

namespace vlx::ui
{
	class VELOX_API TextMesh final : public Batchable<TextMesh>
	{
	private:
		using VertexArray = std::vector<sf::Vertex>;

	public:
		TextMesh() = default;

	public:
		NODISC constexpr sf::PrimitiveType GetPrimitive() const noexcept;

	public:
		void BatchImpl(SpriteBatch& sprite_batch, const Mat4f& transform, float depth) const;

	private:
		const sf::Texture*		m_texture;
		float					m_depth			{0.0f};
		bool					m_draw_outline	{false};

		mutable VertexArray		m_vertices;
		mutable VertexArray		m_outline;

		friend class TextSystem;
	};

	constexpr sf::PrimitiveType	TextMesh::GetPrimitive() const noexcept
	{
		return sf::PrimitiveType::TriangleStrip;
	}
}