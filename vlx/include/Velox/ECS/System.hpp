#pragma once

#include <functional>
#include <span>

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"
#include "Archetype.hpp"

namespace vlx
{
	class EntityAdmin;

	class SystemBase : private NonCopyable
	{
	public:
		friend class EntityAdmin;

	public:
		virtual ~SystemBase() {}

		virtual ArchetypeID GetKey() const = 0;

	protected:
		virtual void DoAction(Time& time, Archetype* archetype) = 0;
	};

	template<class... Cs> requires MustExist<Cs...>
	class System : public SystemBase
	{
	public:
		using Func = typename std::function<void(Time&, std::span<const EntityID>, Cs*...)>;

	public:
		System(EntityAdmin& entity_admin, const std::uint8_t& layer);

		ArchetypeID GetKey() const override;

		void Action(Func&& func);

	protected:
		virtual void DoAction(Time& time, Archetype* archetype) override;

		template<std::size_t Index, typename T, typename... Ts>
		void DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts) requires (Index != sizeof...(Cs));

		template<std::size_t Index, typename T, typename... Ts>
		void DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts) requires (Index == sizeof...(Cs));

	protected:
		EntityAdmin*	m_entity_admin;
		Func			m_func;
		bool			m_func_set{false};
	};

	template<class... Cs> requires MustExist<Cs...>
	inline System<Cs...>::System(EntityAdmin& entity_admin, const std::uint8_t& layer) 
		: m_entity_admin(&entity_admin)
	{
		m_entity_admin->RegisterSystem(layer, this);
	}

	inline ArchetypeID SortKeys(ArchetypeID types)
	{
		std::sort(types.begin(), types.end());
		return types;
	}

	template<class... Cs> requires MustExist<Cs...>
	inline ArchetypeID System<Cs...>::GetKey() const
	{
		return SortKeys({{ Component<Cs>::GetTypeId()... }});
	}	

	template<class... Cs> requires MustExist<Cs...>
	inline void System<Cs...>::Action(Func&& func)
	{
		m_func = std::forward<Func>(func);
		m_func_set = true;
	}


	template<class... Cs> requires MustExist<Cs...>
	inline void System<Cs...>::DoAction(Time& time, Archetype* archetype)
	{
		if (m_func_set)
		{
			DoAction<0>(time, 
				archetype->m_type, 
				archetype->m_entity_ids, 
				archetype->m_component_data);
		}
	}

	template<class... Cs> requires MustExist<Cs...>
	template<std::size_t Index, typename T, typename... Ts>
	inline void System<Cs...>::DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts) requires (Index != sizeof...(Cs))
	{
		using SysCompType = typename std::tuple_element<Index, std::tuple<Cs...>>::type; // get type of element at index in tuple

		std::size_t index2 = 0;

		ComponentTypeID comp_id = Component<SysCompType>::GetTypeId();	// get the id for the type of element at index
		ComponentTypeID archetype_comp_id = archetype_ids[index2];		// id for component in the archetype

		while (comp_id != archetype_comp_id && index2 < archetype_ids.size()) // iterate until matching component is found
		{
			archetype_comp_id = archetype_ids[++index2];
		}

		if (index2 == archetype_ids.size())
			throw std::runtime_error("System was executed against an incorrect Archetype");

		DoAction<Index + 1>(time, archetype_ids, entity_ids, t, ts..., reinterpret_cast<SysCompType*>(&t[index2][0])); // run again on next component, or call final DoAction
	}

	template<class... Cs> requires MustExist<Cs...>
	template<std::size_t Index, typename T, typename... Ts>
	inline void System<Cs...>::DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts) requires (Index == sizeof...(Cs))
	{
		m_func(time, entity_ids, ts...);
	}
}

