#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS/IComponent.h>
#include <Velox/Graphics/Vector2.hpp>
#include <Velox/Graphics/Rectangle.hpp>
#include <Velox/Config.hpp>

#include "IBatchable.h"

namespace vlx
{
	static constexpr std::uint8_t QUAD_COUNT = 4;

	class SpriteBatch;

	///	Custom sprite that is very similar to SFML's except that it has depth 
	///	and no transform as to be suitable for spritebatch and the ECS design
	/// 
	class VELOX_API Sprite : public IComponent, public IBatchable
	{
	private:
		using VertexArray = std::array<sf::Vertex, QUAD_COUNT>;

	public:
		Sprite() = default;
		Sprite(const sf::Texture& texture, const float depth = 0.0f);
		Sprite(const sf::Texture& texture, const Vector2f& size, const float depth = 0.0f);
		Sprite(const sf::Texture& texture, const RectFloat& visible_rect, const float depth = 0.0f);
		Sprite(const sf::Texture& texture, const Vector2f& size, const RectFloat& visible_rect, const float depth = 0.0f);

		NODISC const sf::Texture*			GetTexture() const noexcept;
		NODISC const sf::Shader*			GetShader() const noexcept;
		NODISC const VertexArray&			GetVertices() const noexcept;
		NODISC RectFloat					GetTextureRect() const noexcept;
		NODISC Vector2f						GetSize() const noexcept;
		NODISC float						GetDepth() const noexcept;
		NODISC float						GetOpacity() const noexcept;
		NODISC constexpr sf::PrimitiveType	GetPrimitive() const noexcept;

		void SetTexture(const sf::Texture& texture, bool reset_rect = false, bool reset_size = false);
		void SetTextureRect(const RectFloat& rect);
		void SetSize(const Vector2f& size);
		void SetColor(const sf::Color& color);
		void SetDepth(const float value);
		void SetOpacity(const float opacity);

	public:
		void Batch(SpriteBatch& sprite_batch, const sf::Transform& transform, float depth) const override;

	private:
		void UpdatePositions(const Vector2f& size);
		void UpdateTexCoords(const RectFloat& texture_rect);

	private:
		const sf::Texture*	m_texture		{nullptr};
		const sf::Shader*	m_shader		{nullptr};
		VertexArray			m_vertices;
		float				m_depth			{0.0f};
	};
}