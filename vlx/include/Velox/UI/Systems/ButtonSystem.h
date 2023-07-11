#pragma once

#include <Velox/ECS/System.hpp>
#include <Velox/ECS/SystemAction.h>

#include <Velox/System/Rectangle.hpp>

#include <Velox/Graphics/Components/GlobalTransformTranslation.h>
#include <Velox/UI/Components/Button.h>
#include <Velox/UI/Components/UIBase.h>
#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/World/Object.h>

#include <Velox/Input/MouseCursor.h>
#include <Velox/Window/Camera.h>

#include <Velox/World/EngineBinds.h>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

namespace vlx::ui
{
	class VELOX_API ButtonSystem final : public SystemAction
	{
	private:
		struct ButtonEntityCallback
		{
			EntityID	entity_id {NULL_ENTITY};
			uint8		flags;
		};

	public:
		ButtonSystem(EntityAdmin& entity_admin, LayerType id, 
			const Camera& camera, const EngineMouse& mouse, const MouseCursor& cursor);

	public:
		void Update() override;

	private:
		void ExecuteCallbacks();

		void CallClick(		EntityID entity_id) const;
		void CallPress(		EntityID entity_id) const;
		void CallRelease(	EntityID entity_id) const;
		void CallEnter(		EntityID entity_id) const;
		void CallExit(		EntityID entity_id) const;

	private:
		System<Object, Renderable, GlobalTransformTranslation, UIBase, Button>	m_buttons;
		System<Button> m_check_flags;

		std::vector<ButtonEntityCallback> m_button_callbacks;
	};
}