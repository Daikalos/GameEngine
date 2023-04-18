#pragma once

#include <Velox/Window/CameraBehavior.h>
#include <Velox/Window/Camera.h>

namespace vlx
{
	class CameraZoom final : public CameraBehavior
	{
	public:
		using CameraBehavior::CameraBehavior;

	private:
		void OnCreate(const std::vector<std::byte>& data) override
		{
			m_window = GetContext().window;
			m_mouse_input = &GetContext().controls->Get<MouseInput>();

			m_target_scale = GetCamera().GetScale();
		}

		bool HandleEvent(const sf::Event& event) override
		{ 
			return true;
		}

		bool Update(const Time& time) override
		{
			if (!m_window->hasFocus())
				return true;

			if (m_mouse_input->ScrollUp())
				m_target_scale *= (2.0f - m_zoom_amount);
			if (m_mouse_input->ScrollDown())
				m_target_scale *= m_zoom_amount;

			m_target_scale = Vector2f(
				std::min(std::max(m_target_scale.x, m_min_size), m_max_size),
				std::min(std::max(m_target_scale.y, m_min_size), m_max_size));

			m_current_scale = Vector2f::Lerp(m_current_scale, m_target_scale, 20.0f * time.GetDT());

			GetCamera().SetScale(m_current_scale);

			return true;
		}

	private:
		const Window*		m_window		{nullptr};
		const MouseInput*	m_mouse_input	{nullptr};
		float				m_min_size		{0.1f};
		float				m_max_size		{5.0f};
		float				m_zoom_amount	{0.90f};
		Vector2f			m_current_scale;
		Vector2f			m_target_scale;
	};
}