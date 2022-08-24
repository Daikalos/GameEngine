#pragma once

#include <unordered_map>
#include <unordered_set>

#include "InputHandler.hpp"

namespace fge
{
	////////////////////////////////////////////////////////////
	// Handles all of the joystick input, has support for 
	// several joysticks.
	////////////////////////////////////////////////////////////
	template<Enum JB, Enum JA>
	class JoystickHandler : public InputHandler
	{
	public:
		JoystickHandler()
		{
			m_available_joysticks.rehash(sf::Joystick::Count);

			for (int i = 0; i < sf::Joystick::Count; ++i) // add already available joysticks
				if (sf::Joystick::isConnected(i))
					m_available_joysticks.insert(i);
		}

		void Update(const Time& time, bool focus) override
		{
			for (const uint32_t& i : m_available_joysticks)
			{
				for (uint32_t j = 0; j < sf::Joystick::getButtonCount(i); ++j)
				{
					const uint32_t k = j + i * sf::Joystick::ButtonCount;

					m_previous_button_joystick_state[k] = m_current_button_joystick_state[k];
					m_current_button_joystick_state[k] = focus && m_enabled && sf::Joystick::isButtonPressed(i, j);

					m_joystick_button_held_timer[k] = m_current_button_joystick_state[k] ?
						m_joystick_button_held_timer[k] + (m_joystick_button_held_timer[k] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
				}

				for (uint32_t j = 0; j < sf::Joystick::AxisCount; ++j)
				{
					const uint32_t k = j + i * sf::Joystick::AxisCount;

					sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(j);
					m_joystick_axis[k] = sf::Joystick::hasAxis(i, axis) ? sf::Joystick::getAxisPosition(i, axis) : 0.0f;
				}
			}
		}
		void HandleEvent(const sf::Event& event) override
		{
			switch (event.type)
			{
			case sf::Event::JoystickConnected:
				m_available_joysticks.insert(event.joystickConnect.joystickId);
				break;
			case sf::Event::JoystickDisconnected:
			{
				for (uint32_t i = 0; i < sf::Joystick::ButtonCount; ++i) // be sure to set all to default state
				{
					const uint32_t j = i + event.joystickConnect.joystickId * sf::Joystick::ButtonCount;

					m_previous_button_joystick_state[j] = false;
					m_current_button_joystick_state[j] = false;
					m_joystick_button_held_timer[j] = 0.0f;
				}

				for (uint32_t i = 0; i < sf::Joystick::AxisCount; ++i)
				{
					const uint32_t j = i + event.joystickConnect.joystickId * sf::Joystick::AxisCount;

					m_joystick_axis[j] = 0.0f;
				}

				m_available_joysticks.erase(event.joystickConnect.joystickId);
			}
			break;
			}
		}

	public:
		bool Held(const uint32_t& id, const uint32_t& button) const
		{
			const int index = button + id * sf::Joystick::ButtonCount;
			return m_current_button_joystick_state[index] && m_joystick_button_held_timer[index] >= m_held_threshold;;
		}
		bool Pressed(const uint32_t& id, const uint32_t& button) const
		{
			const int index = button + id * sf::Joystick::ButtonCount;
			return m_current_button_joystick_state[index] && !m_previous_button_joystick_state[index];
		}
		bool Released(const uint32_t& id, const uint32_t& button) const
		{
			return !Pressed(id, button);
		}

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

		float Axis(const uint32_t& id, const uint32_t& axis) const
		{
			return m_joystick_axis[axis + id * sf::Joystick::AxisCount];
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
		std::unordered_set<uint32_t> m_available_joysticks; // list of indices of the currently available joysticks

		bool	m_current_button_joystick_state		[sf::Joystick::Count * sf::Joystick::ButtonCount] = {false};
		bool	m_previous_button_joystick_state	[sf::Joystick::Count * sf::Joystick::ButtonCount] = {false};;
		float	m_joystick_button_held_timer		[sf::Joystick::Count * sf::Joystick::ButtonCount] = {0.0f};;

		float	m_joystick_axis						[sf::Joystick::Count * sf::Joystick::AxisCount] = {0.0f};

		std::unordered_map<JB, uint32_t> m_joystick_button_bindings;	// bindings for joystick buttons
		std::unordered_map<JA, uint32_t> m_joystick_axis_bindings;	// bindings for joystick axis
	};
}