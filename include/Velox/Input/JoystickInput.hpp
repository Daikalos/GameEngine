#pragma once

#include <Velox/Utilities.hpp>

#include <vector>

#include "InputHandler.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Handles all of the joystick input, has support for 
	// several joysticks.
	////////////////////////////////////////////////////////////
	class JoystickInput : public InputHandler
	{
	public:
		JoystickInput()
		{
			m_available.reserve(sf::Joystick::Count);

			for (int i = 0; i < sf::Joystick::Count; ++i) // add already available joysticks
				if (sf::Joystick::isConnected(i))
					m_available.push_back(i);
		}

		void Update(const Time& time, bool focus) override
		{
			for (const uint32_t& i : m_available)
			{
				for (uint32_t j = 0, size = sf::Joystick::getButtonCount(i); j < size; ++j)
				{
					const uint32_t k = j + i * sf::Joystick::ButtonCount;

					m_previous_button_state[k] = m_current_button_state[k];
					m_current_button_state[k] = focus && m_enabled && sf::Joystick::isButtonPressed(i, j);

					m_held_timer[k] = m_current_button_state[k] ?
						m_held_timer[k] + (m_held_timer[k] < m_held_threshold ? time.GetRealDeltaTime() : 0.0f) : 0.0f;
				}

				for (uint32_t j = 0; j < sf::Joystick::AxisCount; ++j)
				{
					const uint32_t k = j + i * sf::Joystick::AxisCount;

					sf::Joystick::Axis axis = static_cast<sf::Joystick::Axis>(j);
					m_axis[k] = sf::Joystick::hasAxis(i, axis) ? sf::Joystick::getAxisPosition(i, axis) : 0.0f;
				}
			}
		}
		void HandleEvent(const sf::Event& event) override
		{
			switch (event.type)
			{
			case sf::Event::JoystickConnected:
				m_available.push_back(event.joystickConnect.joystickId);
				break;
			case sf::Event::JoystickDisconnected:
			{
				const int dest_btn = event.joystickConnect.joystickId * sf::Joystick::ButtonCount;
				const int dest_axis = event.joystickConnect.joystickId * sf::Joystick::AxisCount;

				memset(m_previous_button_state + dest_btn, false, sizeof(bool) * sf::Joystick::ButtonCount); // be sure to set all to default state
				memset(m_current_button_state + dest_btn, false, sizeof(bool) * sf::Joystick::ButtonCount);
				memset(m_held_timer + dest_btn, 0.0f, sizeof(float) * sf::Joystick::ButtonCount);

				memset(m_axis + dest_axis, 0.0f, sizeof(float) * sf::Joystick::AxisCount);

				if (cu::Erase(m_available, event.joystickConnect.joystickId) == m_available.end())
					std::puts("A nonexisting joystick has been asked to be removed");
			}
			break;
			}
		}

	public:
		bool Held(uint32_t id, uint32_t button) const
		{
			const int index = button + id * sf::Joystick::ButtonCount;
			return m_current_button_state[index] && m_held_timer[index] >= m_held_threshold;;
		}
		bool Pressed(uint32_t id, uint32_t button) const
		{
			const int index = button + id * sf::Joystick::ButtonCount;
			return m_current_button_state[index] && !m_previous_button_state[index];
		}
		bool Released(uint32_t id, uint32_t button) const
		{
			return !Pressed(id, button);
		}

		float Axis(uint32_t id, uint32_t axis) const
		{
			return m_axis[axis + id * sf::Joystick::AxisCount];
		}

	private:
		std::vector<uint32_t> m_available; // list of indexes of the currently available joysticks

		bool	m_current_button_state	[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {false};
		bool	m_previous_button_state	[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {false};;
		float	m_held_timer			[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {0.0f};;

		float	m_axis					[sf::Joystick::Count * sf::Joystick::AxisCount]		= {0.0f};
	};
}