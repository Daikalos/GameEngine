#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/ECS/IComponent.h>

#include "IBatchable.h"
#include "Transform.h"

namespace vlx
{
	class SpriteBatch;

	/// <summary>
	///		Custom sprite that is very similar to SFML's except that it has depth 
	///		and no transform as to be suitable for spritebatch and the ECS design
	/// </summary>
	class VELOX_API Sprite : public IComponent, public IBatchable
	{
	private:
		using VertexArray = std::array<sf::Vertex, 4>;
		using TextureRect = sf::Rect<std::uint16_t>; // textures are usually not larger than unsigned short in either width or height

	public:
		Sprite() = default;
		Sprite(const sf::Texture& texture, const float depth = 0.0f);
		Sprite(const sf::Texture& texture, const sf::Vector2f& size, const float depth = 0.0f);
		Sprite(const sf::Texture& texture, const TextureRect& visible_rect, const float depth = 0.0f);
		Sprite(const sf::Texture& texture, const sf::Vector2f& size, const TextureRect& visible_rect, const float depth = 0.0f);

		[[nodiscard]] const sf::Texture* GetTexture() const noexcept;
		[[nodiscard]] const sf::Shader* GetShader() const noexcept;
		[[nodiscard]] const VertexArray& GetVertices() const noexcept;
		[[nodiscard]] const TextureRect& GetTextureRect() const noexcept;
		[[nodiscard]] const sf::Vector2f& GetSize() const noexcept;
		[[nodiscard]] const float& GetDepth() const noexcept;
		[[nodiscard]] const float& GetOpacity() const noexcept;
		[[nodiscard]] constexpr sf::PrimitiveType GetPrimitive() const noexcept;

		void SetTexture(const sf::Texture& texture, bool reset_rect = false, bool reset_size = false);
		void SetTextureRect(const TextureRect& rect);
		void SetSize(const sf::Vector2f& size);
		void SetColor(const sf::Color& color);
		void SetDepth(const float value);
		void SetOpacity(const float opacity);

	public:
		void Batch(SpriteBatch& sprite_batch, const Transform& transform, float depth) const override;

	private:
		void UpdatePositions();
		void UpdateTexCoords();

	private:
		const sf::Texture*	m_texture		{nullptr};
		const sf::Shader*	m_shader		{nullptr};
		VertexArray			m_vertices;
		TextureRect			m_texture_rect;
		sf::Vector2f		m_size;
		float				m_depth			{0.0f};
		float				m_opacity		{1.0f};
	};
}