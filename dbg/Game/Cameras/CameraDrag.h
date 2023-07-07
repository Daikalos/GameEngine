#pragma once

#include <Velox/Window/CameraBehavior.h>
#include <Velox/Window/Camera.h>

#include <Velox/Input/ButtonEvent.hpp>

#include "../Binds.h"

namespace vlx
{
	class CameraDrag final : public CameraBehavior
	{
	public:
		using CameraBehavior::CameraBehavior;

		CameraDrag(ID id, Camera& camera, const Context& context, const GameMouse& mouse) 
			: CameraBehavior(id, camera, context), m_func(mouse), m_mouse_input(&mouse)
		{

		}

	private:
		void OnCreate(const std::vector<std::byte>& data) override
		{
			m_window = GetContext().window;
			m_mouse_cursor = &GetContext().inputs->Cursor();

			m_func.Add(bn::Button::Drag, BT_Pressed, &CameraDrag::Select, this, 1.0f);
			m_func.Add(bn::Button::Drag, BT_Held, &CameraDrag::Drag, this);
		}

		bool HandleEvent(const sf::Event& event) override 
		{ 
			return true; 
		}

		bool Update(const Time& time) override
		{
			//if (pressed || held)
			//{
			//	Vector2f cursor_pos = Vector2f(mouse_cursor.GetPosition())
			//		/ GetCamera().GetScale() / window->GetRatioCmp();

			//	if (pressed)
			//		m_drag_pos = position + cursor_pos;

			//	if (held)
			//	{
			//		if (m_prev_scale != GetCamera().GetScale())
			//			m_drag_pos = position + cursor_pos; //m_drag_pos = position + cursor_pos;

			//		position = (m_drag_pos - cursor_pos);
			//	}

			//	GetCamera().SetPosition(position);
			//	m_prev_scale = GetCamera().GetScale();
			//}

			m_func();

			return true;
		}

		void Select()
		{
			m_drag_pos = Vector2f(m_mouse_cursor->GetPosition());
		}

		void Drag()
		{
			Vector2f mouse_pos = Vector2f(m_mouse_cursor->GetPosition());
			Vector2f offset = m_drag_pos - mouse_pos;
			m_drag_pos = mouse_pos;

			GetCamera().SetPosition(GetCamera().GetPosition() +
				offset / GetCamera().GetScale() / m_window->GetRatioCmp());
		}

	private:
		const Window*		m_window		{nullptr};
		const GameMouse*	m_mouse_input	{nullptr};
		const MouseCursor*  m_mouse_cursor	{nullptr};

		Vector2f m_drag_pos;

		ButtonEvent<GameMouse> m_func;
	};
}