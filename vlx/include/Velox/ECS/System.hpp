#pragma once

#include <span>
#include <functional>
#include <unordered_set>

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"
#include "Archetype.hpp"

namespace vlx
{
	class EntityAdmin;

	class ISystem : private NonCopyable
	{
	public:
		friend class EntityAdmin;

	public:
		virtual ~ISystem() = default;

		virtual bool operator>(const ISystem& rhs) const = 0;

		virtual const ArchetypeID& GetIDKey() const = 0;
		virtual const ComponentIDs& GetArchKey() const = 0;

		virtual float GetPriority() const noexcept = 0;
		virtual void SetPriority(const float val) = 0;

	protected:
		virtual void DoAction(Archetype* archetype) const = 0;
	};

	template<class... Cs> requires IsComponents<Cs...>
	class System : public ISystem
	{
	public:
		using Func = std::function<void(std::span<const EntityID>, Cs*...)>;
		using ComponentTypes = std::tuple<Cs...>;

		using ArchetypeCache = std::unordered_set<ArchetypeID>;

	public:
		System(EntityAdmin& entity_admin, const LayerType layer);
		~System();

		bool operator>(const ISystem& rhs) const override;

	public:
		const ArchetypeID& GetIDKey() const override;
		const ComponentIDs& GetArchKey() const override;

		[[nodiscard]] float GetPriority() const noexcept override;
		void SetPriority(const float val) override;

		void Action(Func&& func);

	public:
		/// <summary>
		///		Exclude any entities that holds these components
		/// </summary>
		template<class... Cs> requires IsComponents<Cs...>
		void Exclude();

	protected:
		virtual void DoAction(Archetype* archetype) const override;

		template<std::size_t Index = 0, typename T, typename... Ts> requires (Index != sizeof...(Cs))
		void DoAction(const ComponentIDs& component_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts) const;

		template<std::size_t Index, typename T, typename... Ts> requires (Index == sizeof...(Cs))
		void DoAction(const ComponentIDs& component_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts) const;

	private:
		bool IsArchetypeExcluded(const Archetype* archetype) const;

	protected:
		EntityAdmin*	m_entity_admin;
		LayerType		m_layer			{LYR_NONE};	// controls the overall order of calls
		Func			m_func;
		float			m_priority		{0.0f};		// priority is for controlling the underlaying order of calls inside a layer

		ComponentIDs			m_exclusion;
		mutable ArchetypeCache	m_excluded_archetypes;

		mutable ArchetypeID		m_id_key	{NULL_ARCHETYPE};
		mutable ComponentIDs	m_arch_key;
	};

	template<class... Cs> requires IsComponents<Cs...>
	inline System<Cs...>::System(EntityAdmin& entity_admin, const LayerType layer)
		: m_entity_admin(&entity_admin), m_layer(layer)
	{
		m_entity_admin->RegisterSystem(m_layer, this);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline System<Cs...>::~System()
	{
		if (m_entity_admin != nullptr)
			m_entity_admin->RemoveSystem(m_layer, this);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline bool System<Cs...>::operator>(const ISystem& rhs) const
	{
		return GetPriority() > rhs.GetPriority();
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline float System<Cs...>::GetPriority() const noexcept
	{
		return m_priority;
	}
	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::SetPriority(const float val)
	{
		m_priority = val;
		m_entity_admin->SortSystems(m_layer);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline const ArchetypeID& System<Cs...>::GetIDKey() const
	{
		if (m_id_key == NULL_ARCHETYPE)
		{
			constexpr auto component_ids	= cu::Sort<ArrComponentIDs<Cs...>>({ ComponentAlloc<Cs>::GetTypeID()... });
			constexpr auto archetype_id		= cu::ContainerHash<ArrComponentIDs<Cs...>>()(component_ids);

			m_id_key = archetype_id;
		}

		return m_id_key;
	}	

	template<class... Cs> requires IsComponents<Cs...>
	inline const ComponentIDs& System<Cs...>::GetArchKey() const
	{
		if (m_arch_key.empty())
		{
			constexpr auto component_ids = cu::Sort<ArrComponentIDs<Cs...>>({ ComponentAlloc<Cs>::GetTypeID()... });
			m_arch_key = { component_ids.begin(), component_ids.end() };
		}

		return m_arch_key;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::Action(Func&& func)
	{
		m_func = std::forward<Func>(func);
	}

	template<class... Cs1> requires IsComponents<Cs1...>
	template<class... Cs2> requires IsComponents<Cs2...>
	inline void System<Cs1...>::Exclude()
	{
		m_exclusion = cu::Sort<ComponentIDs>({ ComponentAlloc<Cs2>::GetTypeID()... });
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::DoAction(Archetype* archetype) const
	{
		if (m_func && !IsArchetypeExcluded(archetype)) // check if func stores callable object and that the archetype is not excluded
		{
			DoAction(
				archetype->type, 
				archetype->entities, 
				archetype->component_data);
		}
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t Index, typename T, typename... Ts> requires (Index != sizeof...(Cs))
	inline void System<Cs...>::DoAction(const ComponentIDs& component_ids, std::span<const EntityID> entity_ids, T& c, Ts... cs) const
	{
		using CompType = std::tuple_element_t<Index, ComponentTypes>;			// get type of component at index in system components
		constexpr ComponentTypeID comp_id = ComponentAlloc<CompType>::GetTypeID();	// get the id for the type of element at index

		std::size_t i = 0;
		ComponentTypeID archetype_comp_id = component_ids[i];

		while (archetype_comp_id != comp_id && i < component_ids.size())	// iterate until matching component is found
		{
			archetype_comp_id = component_ids[++i];
		}

		assert(i != component_ids.size());

		DoAction<Index + 1>(component_ids, entity_ids, c, cs..., reinterpret_cast<CompType*>(&c[i][0])); // run again on next component, or call final DoAction
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t Index, typename T, typename... Ts> requires (Index == sizeof...(Cs))
	inline void System<Cs...>::DoAction(const ComponentIDs& component_ids, std::span<const EntityID> entity_ids, T& t, Ts... ts) const
	{
		m_func(entity_ids, ts...);
	}

	template<class... Cs> requires IsComponents<Cs...>
	bool System<Cs...>::IsArchetypeExcluded(const Archetype* archetype) const
	{
		const auto it = m_excluded_archetypes.find(archetype->id);
		if (it != m_excluded_archetypes.end())
			return true;

		for (const ComponentTypeID component_id : m_exclusion)
		{
			const auto it = cu::FindSorted(archetype->type, component_id);
			if (it != archetype->type.end() && *it == component_id) // if contains excluded component
				return m_excluded_archetypes.insert(archetype->id).second;
		}

		return false;
	}
}

