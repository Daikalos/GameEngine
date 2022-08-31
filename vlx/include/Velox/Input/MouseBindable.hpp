#pragma once

#include <Velox/Utilities.hpp>

#include <unordered_map>

#include "MouseInput.h"
#include "Binds.hpp"

namespace vlx
{
	class MouseBindable final : public MouseInput
	{
	private:
		template<Enum Bind>
		using MouseBinds = Binds<Bind, sf::Mouse::Button>;

	public:
		using MouseInput::Held;
		using MouseInput::Pressed;
		using MouseInput::Released;

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
		MouseBinds<Bind>& Get();
		template<Enum Bind>
		const MouseBinds<Bind>& Get() const;

	private:
		std::unordered_map<std::type_index, BindsBase::Ptr> m_binds;
	};

	template<Enum Bind>
	inline bool MouseBindable::Held(const Bind name) const
	{
		auto& binds = Get<Bind>();
		return binds.GetEnabled() && Held(binds.At(name));
	}
	template<Enum Bind>
	inline bool MouseBindable::Pressed(const Bind name) const
	{
		auto& binds = Get<Bind>();
		return binds.GetEnabled() && Pressed(binds.At(name));
	}
	template<Enum Bind>
	inline bool MouseBindable::Released(const Bind name) const
	{
		auto& binds = Get<Bind>();
		return binds.GetEnabled() && Released(binds.At(name));
	}

	template<Enum Bind>
	inline void MouseBindable::Add()
	{
		m_binds[typeid(Bind)] = BindsBase::Ptr(new MouseBinds<Bind>());
	}

	template<Enum Bind>
	inline MouseBindable::MouseBinds<Bind>& MouseBindable::Get()
	{
		return *static_cast<MouseBinds<Bind>*>(m_binds.at(typeid(Bind)).get()); // is assumed to exist, error otherwise
	}
	template<Enum Bind>
	inline const MouseBindable::MouseBinds<Bind>& MouseBindable::Get() const
	{
		return const_cast<MouseBindable*>(this)->Get<Bind>();
	}
}