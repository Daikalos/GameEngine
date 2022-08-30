#pragma once

#include <Velox/Utilities.hpp>

#include <unordered_map>

#include "MouseInput.hpp"
#include "Binds.hpp"

namespace vlx
{
	class MouseInputBindable final : public MouseInput
	{
	private:
		template<Enum Bind>
		using MouseBinds = Binds<Bind, sf::Mouse::Button>;

	public:
		using MouseInput::Held;
		using MouseInput::Pressed;
		using MouseInput::Released;

		template<Enum Bind>
		bool Held(Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Held(binds.At(name));
		}
		template<Enum Bind>
		bool Pressed(Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Pressed(binds.At(name));
		}
		template<Enum Bind>
		bool Released(Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Released(binds.At(name));
		}

		////////////////////////////////////////////////////////////
		// Add the bind for later input, must be done before any
		// operations are performed using the bind
		////////////////////////////////////////////////////////////
		template<Enum Bind>
		void Add()
		{
			m_binds[typeid(Bind)] = BindsBase::Ptr(new MouseBinds<Bind>());
		}

		template<Enum Bind>
		MouseBinds<Bind>& Get()
		{
			return *static_cast<MouseBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
		}
		template<Enum Bind>
		const MouseBinds<Bind>& Get() const
		{
			return const_cast<MouseInputBindable*>(this)->Get<Bind>();
		}

	private:
		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;
	};
}