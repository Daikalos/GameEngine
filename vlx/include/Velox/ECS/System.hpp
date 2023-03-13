#pragma once

#include <span>
#include <functional>
#include <unordered_set>
#include <cassert>

#include <Velox/Config.hpp>
#include <Velox/System/Event.hpp>
#include <Velox/Utility/NonCopyable.h>
#include <Velox/Utility/ContainerUtils.h>

#include "Identifiers.hpp"
#include "Archetype.hpp"

namespace vlx
{
	class EntityAdmin;

	class ISystem : private NonCopyable
	{
	public:
		virtual ~ISystem() = default;

	public:
		auto operator<=>(const ISystem& rhs) const;

	public:
		virtual ArchetypeID GetIDKey() const = 0;
		virtual const ComponentIDs& GetArchKey() const = 0;

		virtual float GetPriority() const noexcept = 0;
		virtual void SetPriority(const float val) = 0;

		virtual bool IsRunningParallel() const noexcept = 0;
		virtual void SetRunParallel(const bool flag) noexcept = 0;

	protected:
		virtual void Start() const = 0;
		virtual void Run(const Archetype* const archetype) const = 0;

	private:
		friend class EntityAdmin;
	};

	template<class... Cs> requires IsComponents<Cs...>
	class System : public ISystem
	{
	public:
		using AllFunc = std::function<void(std::span<const EntityID>, Cs*...)>;
		using EachFunc = std::function<void(EntityID, Cs&...)>;

		using ComponentTypes = std::tuple<Cs...>;

		using ArchetypeCache = std::unordered_set<ArchetypeID>;

	public:
		static constexpr ArrComponentIDs<Cs...> SystemIDs = 
			cu::Sort<ArrComponentIDs<Cs...>>({ id::Type<Cs>::ID()... }); // another stupid intellisense error

		static constexpr ArchetypeID SystemID =
			cu::ContainerHash<ArrComponentIDs<Cs...>>()(SystemIDs);

	public:
		System() = default;
		System(EntityAdmin& entity_admin, const LayerType layer);

		~System();

	public:
		ArchetypeID GetIDKey() const override;
		const ComponentIDs& GetArchKey() const override;

		NODISC float GetPriority() const noexcept override;
		void SetPriority(const float val) override;

		NODISC bool IsRunningParallel() const noexcept override;
		void SetRunParallel(const bool flag) noexcept override;

		const Archetype* ActiveArchetype() const;

	public:
		void ForceRun();

		void All(AllFunc&& func);
		void Each(EachFunc&& func);

	public:
		/// <summary>
		///		Exclude any entities that holds these components
		/// </summary>
		template<class... Cs> requires IsComponents<Cs...>
		void Exclude();

	protected:
		virtual void Start() const override;
		virtual void Run(const Archetype* const archetype) const override;

		template<std::size_t Index = 0, typename T, typename... Ts> requires (Index != sizeof...(Cs))
		void Run(const ComponentIDs& component_ids, std::span<const EntityID> entities, T& t, Ts... ts) const;

		template<std::size_t Index, typename T, typename... Ts> requires (Index == sizeof...(Cs))
		void Run(const ComponentIDs& component_ids, std::span<const EntityID> entities, T& t, Ts... ts) const;

	private:
		bool IsArchetypeExcluded(const Archetype* archetype) const;

	public:
		Event<> OnStart; // called before system starts being run for each archetype

	protected:	
		EntityAdmin*				m_entity_admin	{nullptr};
		LayerType					m_layer			{LYR_NONE};	// controls the overall order of calls
		AllFunc						m_all_func;
		EachFunc					m_each_func;

		ComponentIDs				m_exclusion;
		mutable ArchetypeCache		m_excluded_archetypes;
		mutable ComponentIDs		m_arch_key;
		mutable const Archetype*	m_archetype		{nullptr};

		float						m_priority		{0.0f};		// priority is for controlling the underlaying order of calls inside a layer
		bool						m_run_parallel	{false};
	};

	inline auto ISystem::operator<=>(const ISystem& rhs) const
	{
		return GetPriority() <=> rhs.GetPriority();
	}

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
	inline bool System<Cs...>::IsRunningParallel() const noexcept
	{
		return m_run_parallel;
	}
	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::SetRunParallel(const bool flag) noexcept
	{
		m_run_parallel = flag;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline const Archetype* System<Cs...>::ActiveArchetype() const
	{
		return m_archetype;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline ArchetypeID System<Cs...>::GetIDKey() const
	{
		return SystemID;
	}	

	template<class... Cs> requires IsComponents<Cs...>
	inline const ComponentIDs& System<Cs...>::GetArchKey() const
	{
		if (m_arch_key.empty())
			m_arch_key = { SystemIDs.begin(), SystemIDs.end() };

		return m_arch_key;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::ForceRun()
	{
		m_entity_admin->RunSystems(m_layer);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::All(AllFunc&& func)
	{
		m_all_func = std::move(func);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::Each(EachFunc&& func)
	{
		m_each_func = std::move(func);
	}

	template<class... Cs1> requires IsComponents<Cs1...>
	template<class... Cs2> requires IsComponents<Cs2...>
	inline void System<Cs1...>::Exclude()
	{
		m_exclusion = cu::Sort<ComponentIDs>({ ComponentAlloc<Cs2>::GetTypeID()... });
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::Start() const
	{
		OnStart();
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::Run(const Archetype* const archetype) const
	{
		if (m_all_func || m_each_func) // check if func stores callable object
		{
			if (IsArchetypeExcluded(archetype)) // check that it is not excluded
				return;

			m_archetype = archetype;

			Run(archetype->type, 
				archetype->entities,
				archetype->component_data);

			m_archetype = nullptr;
		}
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t Index, typename T, typename... Ts> requires (Index != sizeof...(Cs))
	inline void System<Cs...>::Run(const ComponentIDs& component_ids, std::span<const EntityID> entities, T& c, Ts... cs) const
	{
		using ComponentType = std::tuple_element_t<Index, ComponentTypes>; // get type of component at index in system components
		constexpr auto component_id = ComponentAlloc<ComponentType>::GetTypeID();

		std::size_t i = 0;
		ComponentTypeID archetype_comp_id = component_ids[i];
		while (archetype_comp_id != component_id && i < component_ids.size())	// iterate until matching component is found
		{
			archetype_comp_id = component_ids[++i];
		}

		assert(i != component_ids.size());

		Run<Index + 1>(component_ids, entities, c, cs..., reinterpret_cast<ComponentType*>(&c[i][0])); // run again on next component, or call final DoAction
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t Index, typename T, typename... Ts> requires (Index == sizeof...(Cs))
	inline void System<Cs...>::Run(const ComponentIDs& component_ids, std::span<const EntityID> entities, T& t, Ts... ts) const
	{
		if (m_all_func)
			m_all_func(entities, ts...);

		if (m_each_func)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
				m_each_func(entities[i], ts[i]...);
		}
	}

	template<class... Cs> requires IsComponents<Cs...>
	bool System<Cs...>::IsArchetypeExcluded(const Archetype* archetype) const
	{
		if (const auto ait = m_excluded_archetypes.find(archetype->id); ait != m_excluded_archetypes.end())
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

