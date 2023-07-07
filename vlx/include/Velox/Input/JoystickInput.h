#pragma once

#include <array>
#include <unordered_map>

#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Event.hpp>

#include <Velox/Algorithms/FreeVector.hpp>
#include <Velox/Config.hpp>

#include "InputHandler.h"
#include "Binds.hpp"

namespace vlx
{
	///	Handles all of the joystick input, has support for up to 8 joysticks.
	/// 
	class VELOX_API JoystickInput final : public InputHandler
	{
	public:
		using ButtonType = uint32;

	private:
		struct Joystick
		{
			Joystick(uint32 id) : id(id), button_count(sf::Joystick::getButtonCount(id))
			{
				for (uint32 i = 0; i < sf::Joystick::AxisCount; ++i)
					has_axis[i] = sf::Joystick::hasAxis(id, static_cast<sf::Joystick::Axis>(i));
			}

			uint32	id				{0};
			uint32	button_count	{0};

			bool	current_state	[sf::Joystick::ButtonCount] = {false};
			bool	previous_state	[sf::Joystick::ButtonCount]	= {false};
			float	held_time		[sf::Joystick::ButtonCount] = {0.0f};

			float	axis			[sf::Joystick::AxisCount]	= {0.0f};
			bool	has_axis		[sf::Joystick::AxisCount]	= {false};
		};

		using Joysticks = std::array<std::unique_ptr<Joystick>, sf::Joystick::Count>;

	public:
		JoystickInput();

	public:
		NODISC bool Held(		uint32 id, ButtonType button) const;
		NODISC bool Pressed(	uint32 id, ButtonType button) const;
		NODISC bool Released(	uint32 id, ButtonType button) const;
		NODISC float Axis(		uint32 id, sf::Joystick::Axis axis) const;

		std::vector<uint32> GetConnected() const;

		void ConnectAll();
		void DisconnectAll();

	public:
		void Update(const Time& time, const bool focus) override;
		void HandleEvent(const sf::Event& event) override;

	private:
		void Connect(uint32 id);
		void Disconnect(uint32 id);

	private:
		Joysticks m_joysticks;
	};
}