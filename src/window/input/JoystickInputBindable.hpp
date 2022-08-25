#pragma once

#include <unordered_map>
#include <FGE/Concepts.hpp>

#include "JoystickInput.hpp"

namespace fge
{
	template<Enum JB, Enum JA>
	class JoystickInputBindable : public JoystickInput
	{
	public:
		using JoystickInput::Held; // make them visible for overloading
		using JoystickInput::Pressed;
		using JoystickInput::Released;
		using JoystickInput::Axis;

		bool Held(const uint32_t& id, const JB& button) const
		{
			return Held(id, m_joystick_button_bindings.at(button));
		}
		bool Pressed(const uint32_t& id, const JB& name) const
		{
			return Pressed(id, m_joystick_button_bindings.at(name));
		}
		bool Released(const uint32_t& id, const JB& name) const
		{
			return Released(id, m_joystick_button_bindings.at(name));
		}

		float Axis(const uint32_t& id, const JA& axis) const
		{
			return Axis(id, m_joystick_axis_bindings.at(axis));
		}

		void SetButtonBinding(const JB& name, const uint32_t& button)
		{
			m_joystick_button_bindings[name] = button;
		}
		void SetAxisBinding(const JA& name, const uint32_t& axis)
		{
			m_joystick_axis_bindings[name] = axis;
		}

		void RemoveButtonBinding(const JB& button)
		{
			m_joystick_button_bindings.erase(button);
		}
		void RemoveAxisBinding(const JA& axis)
		{
			m_joystick_axis_bindings.erase(axis);
		}

	private:
		std::unordered_map<JB, uint32_t> m_joystick_button_bindings;	// bindings for joystick buttons
		std::unordered_map<JA, uint32_t> m_joystick_axis_bindings;		// bindings for joystick axis
	};
}