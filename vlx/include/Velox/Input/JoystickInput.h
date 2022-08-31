#pragma once

#include <vector>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "InputHandler.h"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Handles all of the joystick input, has support for 
	// several joysticks.
	////////////////////////////////////////////////////////////
	class VELOX_API_EXPORT JoystickInput : public InputHandler
	{
	public:
		JoystickInput();

		void Update(const Time& time, const bool focus) override;
		void HandleEvent(const sf::Event& event) override;

	public:
		bool Held(const uint32_t id, const uint32_t button) const;
		bool Pressed(const uint32_t id, const uint32_t button) const;
		bool Released(const uint32_t id, const uint32_t button) const;

		float Axis(const uint32_t id, const uint32_t axis) const;

	private:
		std::vector<uint32_t> m_available; // list of indexes of the currently available joysticks

		bool	m_current_button_state	[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {false};
		bool	m_previous_button_state	[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {false};;
		float	m_held_timer			[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {0.0f};;

		float	m_axis					[sf::Joystick::Count * sf::Joystick::AxisCount]		= {0.0f};
	};
}