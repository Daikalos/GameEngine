#pragma once

#include "Binds.hpp"
#include "MouseInput.h"

namespace vlx
{
	template<typename Bind> requires (!std::same_as<Bind, sf::Mouse::Button>)
	class MouseBindable : public Binds<Bind, sf::Mouse::Button>
	{
	public:
		using ButtonType = Bind;

	public:
		explicit MouseBindable(const MouseInput& mouse);

	public:
		NODISC bool Held(		const ButtonType& name) const;
		NODISC bool Pressed(	const ButtonType& name) const;
		NODISC bool Released(	const ButtonType& name) const;

	private:
		const MouseInput* m_mouse {nullptr};
	};

	template<typename Bind> requires (!std::same_as<Bind, sf::Mouse::Button>)
	inline MouseBindable<Bind>::MouseBindable(const MouseInput& mouse)
		: m_mouse(&mouse) { }

	template<typename Bind> requires (!std::same_as<Bind, sf::Mouse::Button>)
	inline bool MouseBindable<Bind>::Held(const ButtonType& name) const
	{
		return this->GetEnabled() && m_mouse->Held(this->At(name));
	}

	template<typename Bind> requires (!std::same_as<Bind, sf::Mouse::Button>)
	inline bool MouseBindable<Bind>::Pressed(const ButtonType& name) const
	{
		return this->GetEnabled() && m_mouse->Pressed(this->At(name));
	}

	template<typename Bind> requires (!std::same_as<Bind, sf::Mouse::Button>)
	inline bool MouseBindable<Bind>::Released(const ButtonType& name) const
	{
		return this->GetEnabled() && m_mouse->Released(this->At(name));
	}
}