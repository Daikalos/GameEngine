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
			auto ref = entity_admin.TryGetComponentRef<GlobalTransformTranslation>(entity_id);
			if (ref.has_value())
				m_target = std::move(ref.value());
		}

		void SetOffset(const Vector2f& offset)
		{
			m_offset = offset;
		}

		void EnableSmooth(const bool flag)
		{
			m_should_smooth = flag;
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
			if (!m_target.IsValid())
				return true;

			Vector2f target = m_target->GetPosition() + m_offset;

			if (m_should_smooth)
			{
				Vector2f smooth = Vector2f::Lerp(GetCamera().GetPosition(), target, 
					m_smooth_speed * time.GetDT());

				GetCamera().SetPosition(smooth);
			}
			else
			{
				GetCamera().SetPosition(target);
			}

			return true;
		}

	private:
		ComponentRef<GlobalTransformTranslation> m_target;
		Vector2f m_current;
		Vector2f m_offset;
		float m_smooth_speed {5.0f};
		bool m_should_smooth {true};
	};
}