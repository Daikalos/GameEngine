#pragma once

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

#include <Velox/Graphics/Components/Sprite.h>
#include <Velox/Graphics/Components/Animation.h>
#include <Velox/Graphics/Components/Renderable.h>

#include <Velox/System/Time.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class AnimationSystem final : public SystemAction
	{
	public:
		AnimationSystem(EntityAdmin& entity_admin, LayerType layer, const Time& time);
		
	public:
		void Update() override;

	private:
		void UpdateAnimation(EntityID entity_id, Renderable& renderable, Sprite& sprite, Animation& animation);

	private:
		const Time* m_time {nullptr};

		System<Renderable, Sprite, Animation> m_animation;
	};
}