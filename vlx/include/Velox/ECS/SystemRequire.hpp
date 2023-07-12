#pragma once

#include "System.hpp"

namespace vlx
{
	template<class... Cs> requires IsComponents<Cs...>
	class SystemRequire final : public virtual System<Cs...>
	{
	public:
		using System<Cs...>::System; // bring into scope

	public:
		const ComponentIDs& GetArchKey() const override;

	public:
		///	Require that the entities also holds these components, but don't care about updating them
		/// 
		template<class... Cs2> requires IsComponents<Cs2...> && (!Contains<Cs2, Cs...> && ...)
		void Require();

	private:
		ComponentIDs m_require;
	};

	template<class... Cs> requires IsComponents<Cs...>
	inline const ComponentIDs& SystemRequire<Cs...>::GetArchKey() const
	{
		return m_require;
	}

	template<class... Cs1> requires IsComponents<Cs1...>
	template<class... Cs2> requires IsComponents<Cs2...> && (!Contains<Cs2, Cs1...> && ...)
	inline void SystemRequire<Cs1...>::Require()
	{
		static constexpr ArrComponentIDs<Cs2...> component_ids
			= cu::Sort<ArrComponentIDs<Cs2...>>({ id::Type<Cs2>::ID()... });

		std::ranges::merge(component_ids.begin(), component_ids.end(), 
			System<Cs1...>::SystemIDs.begin(), System<Cs1...>::SystemIDs.end(), m_require.begin());

		// TODO: check if it even works
	}
}