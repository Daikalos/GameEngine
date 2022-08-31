#pragma once

#include <Velox/Utilities.hpp>

#include <unordered_map>

#include "JoystickInput.h"
#include "Binds.hpp"

namespace vlx
{
	class JoystickBindable final : public JoystickInput
	{
	private:
		template<Enum Bind>
		using JoystickBinds = Binds<Bind, std::uint32_t>;

	public:
		using JoystickInput::Held; // make them visible for overloading
		using JoystickInput::Pressed;
		using JoystickInput::Released;
		using JoystickInput::Axis;

		template<Enum Bind>
		bool Held(const std::uint32_t id, const Bind name) const;
		template<Enum Bind>
		bool Pressed(const std::uint32_t id, const Bind name) const;
		template<Enum Bind>
		bool Released(const std::uint32_t id, const Bind name) const;

		template<Enum Bind>
		float Axis(const std::uint32_t id, const Bind name) const;

		////////////////////////////////////////////////////////////
		// Add the bind for later input, must be done before any
		// operations are performed using the bind
		////////////////////////////////////////////////////////////
		template<Enum Bind>
		void Add();

		template<Enum Bind>
		JoystickBinds<Bind>& Get();
		template<Enum Bind>
		const JoystickBinds<Bind>& Get() const;

	private:
		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;
	};

	template<Enum Bind>
	inline bool JoystickBindable::Held(const std::uint32_t id, const Bind name) const
	{
		auto& binds = m_binds.at(typeid(Bind));

		if (!binds.GetEnabled())
			return false;

		return Held(id, binds.At(name));
	}

	template<Enum Bind>
	inline bool JoystickBindable::Pressed(const std::uint32_t id, const Bind name) const
	{
		auto& binds = Get<Bind>();
		return binds.GetEnabled() && Pressed(id, binds.At(name));
	}

	template<Enum Bind>
	inline bool JoystickBindable::Released(const std::uint32_t id, const Bind name) const
	{
		auto& binds = Get<Bind>();
		return binds.GetEnabled() && Released(id, binds.At(name));
	}

	template<Enum Bind>
	inline float JoystickBindable::Axis(const std::uint32_t id, const Bind name) const
	{
		auto& binds = Get<Bind>();
		return binds.GetEnabled() && Axis(id, binds.At(name));
	}

	template<Enum Bind>
	inline void JoystickBindable::Add()
	{
		m_binds[typeid(Bind)] = BindsBase::Ptr(new JoystickBinds<Bind>());
	}

	template<Enum Bind>
	inline JoystickBindable::JoystickBinds<Bind>& JoystickBindable::Get()
	{
		return *static_cast<JoystickBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}

	template<Enum Bind>
	inline const JoystickBindable::JoystickBinds<Bind>& JoystickBindable::Get() const
	{
		return const_cast<JoystickBindable*>(this)->Get<Bind>();
	}
}