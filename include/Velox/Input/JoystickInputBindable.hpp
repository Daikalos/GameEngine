#pragma once

#include <Velox/Utilities.hpp>

#include <unordered_map>

#include "JoystickInput.hpp"
#include "Binds.hpp"

namespace vlx
{
	class JoystickInputBindable final : public JoystickInput
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
		bool Held(std::uint32_t id, Bind name) const
		{
			auto& binds = m_binds.at(typeid(Bind));

			if (!binds.GetEnabled())
				return false;

			return Held(id, binds.At(name));
		}
		template<Enum Bind>
		bool Pressed(std::uint32_t id, Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Pressed(id, binds.At(name));
		}
		template<Enum Bind>
		bool Released(std::uint32_t id, Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Released(id, binds.At(name));
		}

		template<Enum Bind>
		float Axis(std::uint32_t id, Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Axis(id, binds.At(name));
		}

		////////////////////////////////////////////////////////////
		// Add the bind for later input, must be done before any
		// operations are performed using the bind
		////////////////////////////////////////////////////////////
		template<Enum Bind>
		void Add()
		{
			m_binds[typeid(Bind)] = BindsBase::Ptr(new JoystickBinds<Bind>());
		}

		template<Enum Bind>
		JoystickBinds<Bind>& Get()
		{
			return *static_cast<JoystickBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
		}
		template<Enum Bind>
		const JoystickBinds<Bind>& Get() const
		{
			return const_cast<JoystickInputBindable*>(this)->Get<Bind>();
		}

	private:
		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;
	};
}