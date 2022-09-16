#pragma once

#include <Velox/ECS.hpp>
#include <Velox/Graphics/SpriteBatch.h>
#include <Velox/Components/Sprite.h>
#include <Velox/Components/Transform.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API RenderSystem
	{
	private:
		using System = typename System<Sprite, Transform>; // operates on any entity with a sprite and a transform

	public:
		RenderSystem();

		void batch_sprites(bool flag);

	private:
		std::vector<System>	m_systems;
		SpriteBatch			m_sprite_batch;
		bool				m_batch_sprites{true};
	};
}