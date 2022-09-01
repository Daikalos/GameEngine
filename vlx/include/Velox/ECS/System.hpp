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

	template<class... Cs>
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

	template<class... Cs>
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

	template<class... Cs>
	inline ArchetypeID System<Cs...>::GetKey() const
	{
		return SortKeys({{ Component<Cs>::GetTypeId()... }});
	}	

	template<class... Cs>
	inline void System<Cs...>::Action(Func&& func)
	{
		m_func = std::forward<Func>(func);
		m_func_set = true;
	}


	template<class... Cs>
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

	template<class ...Cs>
	template<std::size_t Index, typename T, typename... Ts>
	inline void System<Cs...>::DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts ...ts) requires (Index != sizeof...(Cs))
	{
		using IthT = std::tuple_element<Index, std::tuple<Cs...>>::type; // get type of element at index in tuple

		std::size_t index2 = 0;

		ComponentTypeID this_type_cs = Component<IthT>::GetTypeID();
		ComponentTypeID this_archetype_id = archetype_ids[index2];

		while (this_type_cs != this_archetype_id && index2 < archetype_ids.size())
		{
			this_archetype_id = archetype_ids[++index2];
		}

		if (index2 == archetype_ids.size())
			throw std::runtime_error("System was executed against an incorrect Archetype");

		DoAction<Index + 1>(time, archetype_ids, entity_ids, t, ts..., reinterpret_cast<IthT*>(&t[index2][0]));
	}

	template<class ...Cs>
	template<std::size_t Index, typename T, typename... Ts>
	inline void System<Cs...>::DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts ...ts) requires (Index == sizeof...(Cs))
	{
		m_func(time, entity_ids, ts...);
	}
}

