#pragma once

#include <unordered_map>

#include <Velox/Utilities.hpp>

#include "KeyboardInput.h"
#include "Binds.hpp"

namespace vlx
{
	class KeyboardBindable final : public KeyboardInput
	{
	private:
		template<Enum Bind>
		using KeyboardBinds = Binds<Bind, sf::Keyboard::Key>;

	public:
		using KeyboardInput::Held; // make them visible for overloading
		using KeyboardInput::Pressed;
		using KeyboardInput::Released;

		template<Enum Bind>
		bool Held(const Bind name) const;
		template<Enum Bind>
		bool Pressed(const Bind name) const;
		template<Enum Bind>
		bool Released(const Bind name) const;

		////////////////////////////////////////////////////////////
		// Add the bind for later input, must be done before any
		// operations are performed using the bind
		////////////////////////////////////////////////////////////
		template<Enum Bind>
		void Add();

		template<Enum Bind>
		KeyboardBinds<Bind>& Get();
		template<Enum Bind>
		const KeyboardBinds<Bind>& Get() const;

	private:
		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;
	};

	template<Enum Bind>
	inline bool KeyboardBindable::Held(const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Held(binds.at(name));
	}

	template<Enum Bind>
	inline bool KeyboardBindable::Pressed(const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Pressed(binds.at(name));
	}

	template<Enum Bind>
	inline bool KeyboardBindable::Released(const Bind name) const
	{
		const auto& binds = Get<Bind>();
		return binds.GetEnabled() && Released(binds.at(name));
	}

	template<Enum Bind>
	inline void KeyboardBindable::Add()
	{
		m_binds[typeid(Bind)] = BindsBase::Ptr(new KeyboardBinds<Bind>());
	}

	template<Enum Bind>
	inline KeyboardBindable::KeyboardBinds<Bind>& KeyboardBindable::Get()
	{
		return *static_cast<KeyboardBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}

	template<Enum Bind>
	inline const KeyboardBindable::KeyboardBinds<Bind>& KeyboardBindable::Get() const
	{
		return const_cast<KeyboardBindable*>(this)->Get<Bind>();
	}
}