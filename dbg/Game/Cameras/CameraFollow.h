#pragma once

#include <Velox/Window/CameraBehavior.h>
#include <Velox/Window/Camera.h>
#include <Velox/Input/BtnFunc.hpp>

#include "../Binds.h"

namespace vlx
{
	class CameraFollow final : public CameraBehavior
	{
	public:
		using CameraBehavior::CameraBehavior;

	public:
		void SetTarget(EntityAdmin& entity_admin, EntityID entity_id)
		{
			auto ref = entity_admin.TryGetComponentRef<GlobalTransform>(entity_id);
			if (ref.has_value())
				m_target = std::move(ref.value());
		}

		void SetOffset(const Vector2f& offset)
		{
			m_offset = offset;
		}

	private:
		void OnCreate(const std::vector<std::byte>& data) override
		{
			
		}

		bool HandleEvent(const sf::Event& event) override
		{
			return true;
		}

		bool Update(const Time& time) override
		{
			return true;
		}

		bool PostUpdate(const Time& time) override
		{
			if (m_target.IsValid())
				GetCamera().SetPosition(m_target->GetPosition() + m_offset);

			return true;
		}

	private:
		ComponentRef<GlobalTransform> m_target;
		Vector2f m_offset;
	};
}