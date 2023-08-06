#pragma once

#include "System.hpp"

namespace vlx
{
	template<class S, class... Cs>
	class SystemExclude;

	template<class... Cs1, class... Cs2> requires IsComponents<Cs1...> && IsComponents<Cs2...> && (!Contains<Cs2, Cs1...> && ...)
	class SystemExclude<System<Cs1...>, Cs2...> final : public virtual System<Cs1...>
	{
	private:
		using ExclComponentIDs = ArrComponentIDs<std::remove_const_t<Cs2>...>;

		static constexpr ExclComponentIDs ExcludedIDs =
			cu::Sort<ExclComponentIDs>({ id::Type<std::remove_const_t<Cs2>>::ID()... });

	private:
		struct ArchExclData
		{
			ArchExclData(ArchetypeID id, bool excluded = false) : id(id), excluded(excluded) {};
			bool operator==(const ArchExclData& ae) const { return id == ae.id; }

			ArchetypeID id	{NULL_ARCHETYPE};
			bool excluded	{false};
		};

		using ArchExclCache = std::unordered_set<ArchExclData, decltype([](const ArchExclData& data) { return data.id; })> ;

	public:
		using System<Cs1...>::System;

	protected:
		void Run(const Archetype* const archetype) const override;

	private:
		bool IsArchetypeExcluded(const Archetype* const archetype) const;

	protected:
		mutable ArchExclCache m_excluded_archetypes;
	};

	template<class... Cs1, class... Cs2> requires IsComponents<Cs1...> && IsComponents<Cs2...> && (!Contains<Cs2, Cs1...> && ...)
	inline void SystemExclude<System<Cs1...>, Cs2...>::Run(const Archetype* const archetype) const
	{
		assert(this->IsEnabled() && "System is disabled and cannot be run (EntityAdmin checks for this beforehand)");

		if (this->m_func) // check if func stores callable object
		{
			if (IsArchetypeExcluded(archetype)) // check that it is not excluded
				return;

			System<Cs1...>::Run(
				archetype->type,
				archetype->entities,
				archetype->component_data);
		}
	}

	template<class... Cs1, class... Cs2> requires IsComponents<Cs1...> && IsComponents<Cs2...> && (!Contains<Cs2, Cs1...> && ...)
	inline bool SystemExclude<System<Cs1...>, Cs2...>::IsArchetypeExcluded(const Archetype* const archetype) const
	{
		const auto ait = m_excluded_archetypes.find(archetype->id);
		if (ait != m_excluded_archetypes.end())
			return ait->excluded;

		bool excluded = false;
		for (const ComponentTypeID component_id : ExcludedIDs)
		{
			const auto it = std::upper_bound(archetype->type.begin(), archetype->type.end(), component_id);
			if (it != archetype->type.end() && *it == component_id) // if contains excluded component
			{
				excluded = true;
				break;
			}
		}

		m_excluded_archetypes.emplace(archetype->id, excluded);

		return excluded;
	}
}