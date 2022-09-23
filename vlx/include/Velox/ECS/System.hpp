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
		virtual ~SystemBase() = default;

	public:
		virtual const ArchetypeID& GetKey() const = 0;

		virtual constexpr float GetPriority() const noexcept = 0;
		virtual void SetPriority(const float val) = 0;

	protected:
		virtual void DoAction(Time& time, Archetype* archetype) = 0;
	};

	template<Exists... Cs>
	class System : public SystemBase
	{
	public:
		using Func = typename std::function<void(Time&, std::span<const EntityID>, Cs*...)>;

	public:
		System(EntityAdmin& entity_admin, const std::uint16_t layer);
		~System();

		System(const System& system);
		System& operator=(const System& rhs);

	public:
		const ArchetypeID& GetKey() const override;

		[[nodiscard]] float GetPriority() const noexcept override;
		void SetPriority(const float val) override;

		void Action(Func&& func);

	protected:
		virtual void DoAction(Time& time, Archetype* archetype) override;

		template<std::size_t Index, typename T, typename... Ts> requires (Index != sizeof...(Cs))
		void DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts);

		template<std::size_t Index, typename T, typename... Ts> requires (Index == sizeof...(Cs))
		void DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts);

	protected:
		EntityAdmin*	m_entity_admin;
		std::uint16_t	m_layer			{0};	// controls the overall order of calls
		float			m_priority		{0.0f}; // priority is for controlling the underlaying order of calls inside a layer

		Func			m_func;
		bool			m_func_set		{false};

		mutable ArchetypeID m_key;
	};

	template<Exists... Cs>
	inline System<Cs...>::System(EntityAdmin& entity_admin, const std::uint16_t layer)
		: m_entity_admin(&entity_admin), m_layer(layer)
	{
		m_entity_admin->RegisterSystem(m_layer, this);
	}

	template<Exists... Cs>
	inline System<Cs...>::~System()
	{
		if (m_entity_admin)
			m_entity_admin->RemoveSystem(m_layer, this);
	}

	template<Exists... Cs>
	inline System<Cs...>::System(const System<Cs...>& system)
		:	m_entity_admin(system.m_entity_admin), m_layer(system.m_layer), m_priority(system.m_priority),
			m_func(system.m_func), m_func_set(system.m_func_set), m_key(system.m_key) { }

	template<Exists... Cs>
	inline System<Cs...>& System<Cs...>::operator=(const System<Cs...>& rhs)
	{
		m_entity_admin = rhs.m_entity_admin;
		m_layer = rhs.m_layer;
		m_priority = rhs.m_priority;

		m_func = rhs.m_func;
		m_func_set = rhs.m_func_set;

		m_key = rhs.m_key;

		return *this;
	}

	template<Exists... Cs>
	inline float System<Cs...>::GetPriority() const noexcept
	{
		return m_priority;
	}
	template<Exists... Cs>
	inline void System<Cs...>::SetPriority(const float val)
	{
		m_priority = val;
		m_entity_admin->SortSystems(m_layer);
	}

	inline ArchetypeID SortKeys(ArchetypeID types)
	{
		std::sort(types.begin(), types.end());
		return types;
	}

	template<Exists... Cs>
	inline const ArchetypeID& System<Cs...>::GetKey() const
	{
		if (m_key.empty())
			m_key = SortKeys({{ Component<Cs>::GetTypeId()... }});

		return m_key;
	}	

	template<Exists... Cs>
	inline void System<Cs...>::Action(Func&& func)
	{
		m_func = std::forward<Func>(func);
		m_func_set = true;
	}

	template<Exists... Cs>
	inline void System<Cs...>::DoAction(Time& time, Archetype* archetype)
	{
		if (m_func_set)
		{
			DoAction<0>(time, 
				archetype->type, 
				archetype->entities, 
				archetype->component_data);
		}
	}

	template<Exists... Cs>
	template<std::size_t Index, typename T, typename... Ts> requires (Index != sizeof...(Cs))
	inline void System<Cs...>::DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts)
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

	template<Exists... Cs>
	template<std::size_t Index, typename T, typename... Ts> requires (Index == sizeof...(Cs))
	inline void System<Cs...>::DoAction(Time& time, const ArchetypeID& archetype_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts)
	{
		m_func(time, entity_ids, ts...);
	}
}

