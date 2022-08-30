#pragma once

#include <Velox/Utilities.hpp>

#include <unordered_map>

#include "KeyboardInput.hpp"
#include "Binds.hpp"

namespace vlx
{
	class KeyboardInputBindable final : public KeyboardInput
	{
	private:
		template<Enum Bind>
		using KeyboardBinds = Binds<Bind, sf::Keyboard::Key>;

	public:
		using KeyboardInput::Held; // make them visible for overloading
		using KeyboardInput::Pressed;
		using KeyboardInput::Released;

		template<Enum Bind>
		bool Held(Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Held(binds.at(name));
		}
		template<Enum Bind>
		bool Pressed(Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Pressed(binds.at(name));
		}
		template<Enum Bind>
		bool Released(Bind name) const
		{
			auto& binds = Get<Bind>();
			return binds.GetEnabled() && Released(binds.at(name));
		}

		////////////////////////////////////////////////////////////
		// Add the bind for later input, must be done before any
		// operations are performed using the bind
		////////////////////////////////////////////////////////////
		template<Enum Bind>
		void Add()
		{
			m_binds[typeid(Bind)] = BindsBase::Ptr(new KeyboardBinds<Bind>());
		}

		template<Enum Bind>
		KeyboardBinds<Bind>& Get()
		{
			return *static_cast<KeyboardBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
		}
		template<Enum Bind>
		const KeyboardBinds<Bind>& Get() const
		{
			return const_cast<KeyboardInputBindable*>(this)->Get<Bind>();
		}

	private:
		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;
	};
}