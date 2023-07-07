#pragma once

#include "Binds.hpp"
#include "KeyboardInput.h"

namespace vlx
{
	template<typename Bind> requires (!std::same_as<Bind, sf::Keyboard::Key>)
	class KeyboardBindable : public Binds<Bind, sf::Keyboard::Key>
	{
	public:
		using ButtonType = Bind;

	public:
		explicit KeyboardBindable(const KeyboardInput& keyboard);

	public:
		NODISC bool Held(		const ButtonType& name) const;
		NODISC bool Pressed(	const ButtonType& name) const;
		NODISC bool Released(	const ButtonType& name) const;

	private:
		const KeyboardInput* m_keyboard {nullptr};
	};

	template<typename Bind> requires (!std::same_as<Bind, sf::Keyboard::Key>)
	inline KeyboardBindable<Bind>::KeyboardBindable(const KeyboardInput& keyboard)
		: m_keyboard(&keyboard) { }

	template<typename Bind> requires (!std::same_as<Bind, sf::Keyboard::Key>)
	inline bool KeyboardBindable<Bind>::Held(const ButtonType& name) const
	{
		return this->GetEnabled() && m_keyboard->Held(this->At(name));
	}

	template<typename Bind> requires (!std::same_as<Bind, sf::Keyboard::Key>)
	inline bool KeyboardBindable<Bind>::Pressed(const ButtonType& name) const
	{
		return this->GetEnabled() && m_keyboard->Pressed(this->At(name));
	}

	template<typename Bind> requires (!std::same_as<Bind, sf::Keyboard::Key>)
	inline bool KeyboardBindable<Bind>::Released(const ButtonType& name) const
	{
		return this->GetEnabled() && m_keyboard->Released(this->At(name));
	}
}