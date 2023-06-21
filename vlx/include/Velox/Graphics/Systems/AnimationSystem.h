#pragma once

#include <Velox/ECS.hpp>

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
		void Start() override;

		void PreUpdate() override;
		void Update() override;
		void FixedUpdate() override;
		void PostUpdate() override;

		void Draw(Window& window) const override;
		void DrawGUI(Window& window) const override;

	private:
		System<Renderable, Sprite, Animation> m_animation;
	};
}