#pragma once

#include <vector>
#include <unordered_map>

#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Event.hpp>

#include <Velox/Config.hpp>

#include "InputHandler.h"
#include "Binds.hpp"

namespace vlx
{
	///	Handles all of the joystick input, has support for up to 8 joysticks.
	/// 
	class JoystickInput final : public InputHandler
	{
	private:
		using SizeType = std::uint16_t;

		template<Enum Bind>
		using JoystickBinds = Binds<Bind, SizeType>;

	public:
		VELOX_API JoystickInput();

	public:
		VELOX_API NODISC bool Held(const SizeType id, const SizeType button) const;
		VELOX_API NODISC bool Pressed(const SizeType id, const SizeType button) const;
		VELOX_API NODISC bool Released(const SizeType id, const SizeType button) const;

		VELOX_API NODISC float Axis(const SizeType id, const SizeType axis) const;

	public:
		template<Enum Bind>
		NODISC bool Held(const SizeType id, const Bind name) const;
		template<Enum Bind>
		NODISC bool Pressed(const SizeType id, const Bind name) const;
		template<Enum Bind>
		NODISC bool Released(const SizeType id, const Bind name) const;

		template<Enum Bind>
		NODISC float Axis(const SizeType id, const Bind name) const;

		template<Enum Bind>
		NODISC JoystickBinds<Bind>& GetMap();
		template<Enum Bind>
		NODISC const JoystickBinds<Bind>& GetMap() const;

		///	Add the bind for later input, must be done before any operations are performed using the bind
		/// 
		template<Enum Bind>
		void AddMap();

	public:
		VELOX_API void Update(const Time& time, const bool focus) override;
		VELOX_API void HandleEvent(const sf::Event& event) override;

	private:
		bool	m_current_state		[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {false};
		bool	m_previous_state	[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {false};
		float	m_held_time			[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {0.0f};

		float	m_axis				[sf::Joystick::Count * sf::Joystick::AxisCount]		= {0.0f};

		bool	m_available			[sf::Joystick::Count] = {false}; // array of bools indicating currently available joysticks

		std::unordered_map<std::type_index, IBinds::Ptr> m_binds;
	};

	template<Enum Bind>
	inline bool JoystickInput::Held(const SizeType id, const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Held(id, binds.At(name));
	}
	template<Enum Bind>
	inline bool JoystickInput::Pressed(const SizeType id, const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Pressed(id, binds.At(name));
	}
	template<Enum Bind>
	inline bool JoystickInput::Released(const SizeType id, const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Released(id, binds.At(name));
	}

	template<Enum Bind>
	inline float JoystickInput::Axis(const SizeType id, const Bind name) const
	{
		const auto& binds = GetMap<Bind>();
		return binds.GetEnabled() && Axis(id, binds.At(name));
	}

	template<Enum Bind>
	inline JoystickInput::JoystickBinds<Bind>& JoystickInput::GetMap()
	{
		return *static_cast<JoystickBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}
	template<Enum Bind>
	inline const JoystickInput::JoystickBinds<Bind>& JoystickInput::GetMap() const
	{
		return const_cast<JoystickInput*>(this)->GetMap<Bind>();
	}

	template<Enum Bind>
	inline void JoystickInput::AddMap()
	{
		m_binds[typeid(Bind)] = std::make_unique<JoystickBinds<Bind>>();
	}
}