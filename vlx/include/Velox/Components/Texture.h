#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>
#include <Velox/Utilities.hpp>

namespace vlx
{
	class VELOX_API Texture
	{
	public:
		Texture();
		Texture(const sf::Texture& texture, float depth = 0.0f);
		Texture(const sf::Texture& texture, const sf::IntRect& rect, float depth = 0.0f);

	public:
		[[nodiscard]] const sf::Texture* GetTexture() const noexcept;
		[[nodiscard]] const sf::VertexArray& GetVertices() const noexcept;
		[[nodiscard]] constexpr const float GetDepth() const noexcept;

		void SetTexture(const sf::Texture& texture);
		void SetTextureRect(const sf::IntRect& rect);
		void SetColor(const sf::Color& color);
		void SetDepth(const float value) noexcept;

	private:
		void UpdatePositions();
		void UpdateTexCoords();

	private:
		const sf::Texture*	m_texture{nullptr};
		sf::VertexArray		m_vertices;
		sf::IntRect			m_texture_rect;
		float				m_depth{0.0f};
	};
}