#pragma once

#include "System.hpp"

namespace vlx
{
	template<class S, class... Cs>
	class SystemRequire;

	template<class... Cs1, class... Cs2> requires IsComponents<Cs1...> && IsComponents<Cs2...> && (!Contains<Cs2, Cs1...> && ...)
	class SystemRequire<System<Cs1...>, Cs2...> final : public virtual System<Cs1...>
	{
	private:
		using ReqComponentIDs		= ArrComponentIDs<std::remove_const_t<Cs2>...>;
		using MergedComponentIDs	= ArrComponentIDs<std::remove_const_t<Cs1>..., std::remove_const_t<Cs2>...>;

		static constexpr ReqComponentIDs RequireIDs =
			cu::Sort<ReqComponentIDs>({ id::Type<std::remove_const_t<Cs2>>::ID()... });

		static constexpr MergedComponentIDs MergedIDs =
			cu::Merge<typename System<Cs1...>::ArrComponentIDs, ReqComponentIDs, MergedComponentIDs>(System<Cs1...>::SystemIDs, RequireIDs);

	public:
		using System<Cs1...>::System; // bring into scope

	public:
		ComponentIDSpan GetArchKey() const override;
	};

	template<class... Cs1, class... Cs2> requires IsComponents<Cs1...>&& IsComponents<Cs2...> && (!Contains<Cs2, Cs1...> && ...)
	inline ComponentIDSpan SystemRequire<System<Cs1...>, Cs2...>::GetArchKey() const
	{
		return MergedIDs; // TODO: check if it even works
	}
}