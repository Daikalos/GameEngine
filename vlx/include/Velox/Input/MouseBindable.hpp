#pragma once

#include "Binds.hpp"
#include "MouseInput.h"

namespace vlx
{
	template<typename Bind>
	class MouseBindable : public Binds<Bind, sf::Mouse::Button>
	{
	public:
		using ButtonType = Bind;

	public:
		MouseBindable(const MouseInput& mouse);

	public:
		NODISC bool Held(		const ButtonType& name) const;
		NODISC bool Pressed(	const ButtonType& name) const;
		NODISC bool Released(	const ButtonType& name) const;

	private:
		const MouseInput* m_mouse {nullptr};
	};

	template<typename Bind>
	inline MouseBindable<Bind>::MouseBindable(const MouseInput& mouse)
		: m_mouse(&mouse) { }

	template<typename Bind>
	inline bool MouseBindable<Bind>::Held(const ButtonType& name) const
	{
		return this->GetEnabled() && m_mouse->Held(this->At(name));
	}

	template<typename Bind>
	inline bool MouseBindable<Bind>::Pressed(const ButtonType& name) const
	{
		return this->GetEnabled() && m_mouse->Pressed(this->At(name));
	}

	template<typename Bind>
	inline bool MouseBindable<Bind>::Released(const ButtonType& name) const
	{
		return this->GetEnabled() && m_mouse->Released(this->At(name));
	}
}