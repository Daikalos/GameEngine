#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>
#include <Velox/Graphics/Resources.h>
#include <Velox/Graphics/SpriteBatch.h>

namespace vlx
{
	class VELOX_API Drawable
	{
	public:
		Drawable();
		~Drawable();

		void Draw(SpriteBatch& sprite_batch);

	private:
		sf::Texture m_texture;
		Texture::ID m_texture_id;
		bool		m_texture_loaded;
		float		m_depth;
	};
}