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

	////////////////////////////////////////////////////////////
	// Custom sprite that is very similar to SFML's except that
	// it has depth and no transform as to be suitable for 
	// spritebatch and the ECS design
	////////////////////////////////////////////////////////////
	class VELOX_API Sprite : public IComponent, public IBatchable
	{
	private:
		using VertexArray = std::array<sf::Vertex, 4>;

	public:
		Sprite();
		Sprite(const sf::Texture& texture, float depth = 0.0f);
		Sprite(const sf::Texture& texture, const sf::IntRect& rect, float depth = 0.0f);

		void Batch(SpriteBatch& sprite_batch, const Transform& transform, float depth) const override;

	public:
		[[nodiscard]] const sf::Texture* GetTexture() const noexcept;
		[[nodiscard]] const sf::Shader* GetShader() const noexcept;
		[[nodiscard]] const VertexArray& GetVertices() const noexcept;
		[[nodiscard]] const sf::IntRect& GetTextureRect() const noexcept;
		[[nodiscard]] const float& GetDepth() const noexcept;

		[[nodiscard]] const sf::FloatRect GetLocalBounds() const noexcept;
		[[nodiscard]] const sf::Vector2f GetSize() const noexcept;
		[[nodiscard]] constexpr sf::PrimitiveType GetPrimitive() const noexcept;

		void SetTexture(const sf::Texture& texture, bool reset_rect = false);
		void SetTextureRect(const sf::IntRect& rect);
		void SetColor(const sf::Color& color);
		void SetDepth(const float value) noexcept;
		void SetBatch(const bool flag);

	private:
		void UpdatePositions();
		void UpdateTexCoords();

	private:
		const sf::Texture*	m_texture		{nullptr};
		const sf::Shader*	m_shader		{nullptr};
		VertexArray			m_vertices;
		sf::IntRect			m_texture_rect;
		float				m_depth			{0.0f};
		bool				m_batch			{true};
	};
}