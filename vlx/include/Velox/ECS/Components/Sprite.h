#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API Sprite
	{
	public:
		Sprite(const sf::Texture* texture, float depth = 0.0f);
		~Sprite();

	public:
		[[nodiscard]] const sf::Texture* GetTexture() const noexcept;
		[[nodiscard]] constexpr const float GetDepth() const noexcept;

		void SetDepth(float value) noexcept;

	public:
		void Draw();

	private:
		sf::Sprite			m_sprite;
		float				m_depth{0.0f};
	};
}