#pragma once

#include <array>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Rectangle.hpp>
#include <Velox/VeloxTypes.hpp>
#include <Velox/Config.hpp>

#include "IBatchable.h"

namespace vlx
{
	static constexpr uint8 QUAD_COUNT = 4;

	class SpriteBatch;

	///	Custom sprite that is very similar to SFML's except that it has depth 
	///	and no transform as to be suitable for spritebatch and the ECS design
	/// 
	class VELOX_API Sprite : public IBatchable
	{
	private:
		using VertexArray = std::array<sf::Vertex, QUAD_COUNT>;

	public:
		Sprite() = default;
		Sprite(const sf::Texture& texture, float depth = 0.0f);
		Sprite(const sf::Texture& texture, const Vector2f& size, float depth = 0.0f);
		Sprite(const sf::Texture& texture, const RectFloat& visible_rect, float depth = 0.0f);
		Sprite(const sf::Texture& texture, const Vector2f& size, const RectFloat& visible_rect, float depth = 0.0f);

		NODISC const sf::Texture*			GetTexture() const noexcept;
		NODISC const sf::Shader*			GetShader() const noexcept;
		NODISC const VertexArray&			GetVertices() const noexcept;
		NODISC RectFloat					GetTextureRect() const noexcept;
		NODISC Vector2f						GetSize() const noexcept;
		NODISC float						GetDepth() const noexcept;
		NODISC float						GetOpacity() const noexcept;
		NODISC constexpr sf::PrimitiveType	GetPrimitive() const noexcept;

		void SetTexture(const sf::Texture& texture, bool reset_rect = true, bool reset_size = false);
		void SetTextureRect(const RectFloat& rect);
		void SetSize(const Vector2f& size);
		void SetColor(const sf::Color& color);
		void SetDepth(float value);
		void SetOpacity(float opacity);

	public:
		void Batch(SpriteBatch& sprite_batch, const Mat4f& transform, float depth) const override;

	private:
		void UpdatePositions(const Vector2f& size);
		void UpdateTexCoords(const RectFloat& texture_rect);

	private:
		const sf::Texture*	m_texture	{nullptr};
		const sf::Shader*	m_shader	{nullptr};
		VertexArray			m_vertices;
		float				m_depth		{0.0f};
	};

	constexpr sf::PrimitiveType Sprite::GetPrimitive() const noexcept
	{
		return sf::TriangleStrip; // all common sprites use triangle strip
	}
}