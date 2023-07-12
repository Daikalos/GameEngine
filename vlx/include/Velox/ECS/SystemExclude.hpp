#pragma once

#include "System.hpp"

namespace vlx
{
	template<class... Cs> requires IsComponents<Cs...>
	class SystemExclude final : public virtual System<Cs...>
	{
	private:
		struct ArchExclData
		{
			ArchExclData(ArchetypeID id, bool excluded = false) : id(id), excluded(excluded) {};
			bool operator==(const ArchExclData& ae) const { return id == ae.id; }

			ArchetypeID id{ NULL_ARCHETYPE };
			bool excluded{ false };
		};

		using ArchExclCache = std::unordered_set < ArchExclData, decltype([](const ArchExclData& data) { return data.id; }) > ;

	public:
		using System<Cs...>::System;

	public:
		///	Exclude any entities that holds these components
		/// 
		template<class... Cs2> requires IsComponents<Cs2...> && (!Contains<Cs2, Cs...> && ...)
			void Exclude();

	protected:
		void Run(const Archetype* const archetype) const override;

	private:
		bool IsArchetypeExcluded(const Archetype* const archetype) const;

	protected:
		ComponentIDs			m_exclusion;
		mutable ArchExclCache	m_excluded_archetypes;
	};

	template<class... Cs1> requires IsComponents<Cs1...>
	template<class... Cs2> requires IsComponents<Cs2...> && (!Contains<Cs2, Cs1...> && ...)
	inline void SystemExclude<Cs1...>::Exclude()
	{
		constexpr ArrComponentIDs<Cs2...> component_ids 
			= cu::Sort<ArrComponentIDs<Cs2...>>({ id::Type<Cs2>::ID()... });

		m_exclusion = { component_ids.begin(), component_ids.end() };
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void SystemExclude<Cs...>::Run(const Archetype* const archetype) const
	{
		assert(this->IsEnabled() && "System is disabled and cannot be run (EntityAdmin checks for this beforehand)");

		if (this->m_func) // check if func stores callable object
		{
			if (IsArchetypeExcluded(archetype)) // check that it is not excluded
				return;

			System<Cs...>::Run(
				archetype->type,
				archetype->entities,
				archetype->component_data);
		}
	}

	template<class ...Cs> requires IsComponents<Cs...>
	inline bool SystemExclude<Cs...>::IsArchetypeExcluded(const Archetype* const archetype) const
	{
		const auto ait = m_excluded_archetypes.find(archetype->id);
		if (ait != m_excluded_archetypes.end())
			return ait->excluded;

		bool excluded = false;
		for (const ComponentTypeID component_id : m_exclusion)
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