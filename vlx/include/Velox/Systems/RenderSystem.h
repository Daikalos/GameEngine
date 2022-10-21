#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>

#include <Velox/Components/GameObject.h>
#include <Velox/Components/Sprite.h>
#include <Velox/Components/Transform.h>

#include <Velox/Graphics/SpriteBatch.h>

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API RenderSystem : public sf::Drawable
	{
	private:
		using System = System<GameObject, Transform, Sprite>;

	public:
		RenderSystem(EntityAdmin& entity_admin);

		void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

		void UpdateStaticBatch();

		void SetBatchMode(const BatchMode batch_mode);
		void SetBatchingEnabled(const bool flag);

	private:
		EntityAdmin*		m_entity_admin;

		System				m_static_system;
		SpriteBatch			m_static_batch;

		System				m_dynamic_system;
		SpriteBatch			m_dynamic_batch;

		bool				m_batching_enabled{true};
	};
}