#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/Graphics/Resources.h>
#include <Velox/Graphics/SpriteBatch.h>

namespace vlx
{
	class VELOX_API Drawable
	{
	public:
		Drawable(const sf::Texture* texture, float depth = 0.0f);
		~Drawable();

	public:
		[[nodiscard]] constexpr const sf::Texture* const GetTexture() const noexcept;
		[[nodiscard]] constexpr const float GetDepth() const noexcept;

		void SetDepth(float value) noexcept;

	public:
		void Draw();

	private:
		sf::Sprite			m_sprite;
		float				m_depth{0.0f};
	};
}