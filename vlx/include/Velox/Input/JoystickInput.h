#pragma once

#include <vector>
#include <unordered_map>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "InputHandler.h"
#include "Binds.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// Handles all of the joystick input, has support for 
	// several joysticks.
	////////////////////////////////////////////////////////////
	class JoystickInput final : public InputHandler
	{
	private:
		template<Enum Bind>
		using JoystickBinds = Binds<Bind, std::uint32_t>;

	public:
		VELOX_API JoystickInput();

		VELOX_API void Update(const Time& time, const bool focus) override;
		VELOX_API void HandleEvent(const sf::Event& event) override;

	public:
		VELOX_API [[nodiscard]] bool Held(const uint32_t id, const uint32_t button) const;
		VELOX_API [[nodiscard]] bool Pressed(const uint32_t id, const uint32_t button) const;
		VELOX_API [[nodiscard]] bool Released(const uint32_t id, const uint32_t button) const;

		VELOX_API [[nodiscard]] float Axis(const uint32_t id, const uint32_t axis) const;

		template<Enum Bind>
		[[nodiscard]] bool Held(const std::uint32_t id, const Bind name) const;
		template<Enum Bind>
		[[nodiscard]] bool Pressed(const std::uint32_t id, const Bind name) const;
		template<Enum Bind>
		[[nodiscard]] bool Released(const std::uint32_t id, const Bind name) const;

		template<Enum Bind>
		[[nodiscard]] float Axis(const std::uint32_t id, const Bind name) const;

		template<Enum Bind>
		[[nodiscard]] JoystickBinds<Bind>& Get();
		template<Enum Bind>
		[[nodiscard]] const JoystickBinds<Bind>& Get() const;

		////////////////////////////////////////////////////////////
		// Add the bind for later input, must be done before any
		// operations are performed using the bind
		////////////////////////////////////////////////////////////
		template<Enum Bind>
		void Add();

	private:
		std::vector<uint32_t> m_available; // list of indexes of the currently available joysticks

		bool	m_current_state		[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {false};
		bool	m_previous_state	[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {false};
		float	m_held_time			[sf::Joystick::Count * sf::Joystick::ButtonCount]	= {0.0f};

		float	m_axis				[sf::Joystick::Count * sf::Joystick::AxisCount]		= {0.0f};

		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;
	};

	template<Enum Bind>
	[[nodiscard]] inline bool JoystickInput::Held(const std::uint32_t id, const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Held(id, binds.At(name));
	}
	template<Enum Bind>
	[[nodiscard]] inline bool JoystickInput::Pressed(const std::uint32_t id, const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Pressed(id, binds.At(name));
	}
	template<Enum Bind>
	[[nodiscard]] inline bool JoystickInput::Released(const std::uint32_t id, const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Released(id, binds.At(name));
	}

	template<Enum Bind>
	[[nodiscard]] inline float JoystickInput::Axis(const std::uint32_t id, const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Axis(id, binds.At(name));
	}

	template<Enum Bind>
	[[nodiscard]] inline JoystickInput::JoystickBinds<Bind>& JoystickInput::Get()
	{
		return *static_cast<JoystickBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}
	template<Enum Bind>
	[[nodiscard]] inline const JoystickInput::JoystickBinds<Bind>& JoystickInput::Get() const
	{
		return const_cast<JoystickInput*>(this)->Get<Bind>();
	}

	template<Enum Bind>
	inline void JoystickInput::Add()
	{
		m_binds[typeid(Bind)] = std::make_unique<JoystickBinds<Bind>>();
	}
}