#pragma once

#include <Velox/System/Event.hpp>

#include "System.hpp"

namespace vlx
{
	template<class... Cs> requires IsComponents<Cs...>
	class SystemEvent final : public virtual System<Cs...>
	{
	public:
		using System<Cs...>::System;

	public:
		void Start() const override;
		void End() const override;

	public:
		Event<> OnStart;	// called before system starts being run for each archetype
		Event<> OnEnd;		// called after system has run for each archetype
	};

	template<class... Cs> requires IsComponents<Cs...>
	inline void SystemEvent<Cs...>::Start() const
	{
		OnStart();
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void SystemEvent<Cs...>::End() const
	{
		OnEnd();
	}
}