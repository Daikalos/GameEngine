#pragma once

#include <span>
#include <functional>
#include <unordered_set>
#include <cassert>

#include <Velox/Config.hpp>
#include <Velox/System/Event.hpp>
#include <Velox/System/IDGenerator.h>
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

		float GetPriority() const noexcept;
		bool IsRunningParallel() const noexcept;
		bool IsEnabled() const noexcept;

		virtual void SetPriority(const float val);
		virtual void SetRunParallel(const bool flag);
		virtual void SetEnabled(const bool flag);

	protected:
		virtual void Start() const = 0;
		virtual void Run(const Archetype* const archetype) const = 0;

	private:
		float	m_priority		{0.0f};		// priority is for controlling the underlaying order of calls inside a layer
		bool	m_run_parallel	{false};
		bool	m_enabled		{true};

		friend class EntityAdmin;
	};

	template<class... Cs> requires IsComponents<Cs...>
	class System : public ISystem
	{
	public:
		using AllFunc			= std::function<void(std::span<const EntityID>, Cs*...)>;
		using EachFunc			= std::function<void(EntityID, Cs&...)>;
		using ComponentTypes	= std::tuple<Cs...>;

		static constexpr ArrComponentIDs<Cs...> SystemIDs =
			cu::Sort<ArrComponentIDs<Cs...>>({ id::Type<Cs>::ID()... }); // another stupid intellisense error

		static constexpr ArchetypeID SystemID =
			cu::ContainerHash<ArrComponentIDs<Cs...>>()(SystemIDs);

	private:
		struct ArchExclude
		{
			ArchExclude(ArchetypeID id, bool excluded = false) : id(id), excluded(excluded) {};

			ArchetypeID id;
			bool excluded;

			constexpr bool operator==(const ArchExclude& ae) const
			{
				return id == ae.id;
			}
		};

		struct HashAE
		{
			constexpr std::size_t operator()(const ArchExclude& ae) const
			{
				return ae.id; // might as well just return id, as it will always be unique *I hope*
			}
		};

		using ArchExclCache = std::unordered_set<ArchExclude, HashAE>;

	public:
		System() = default;
		System(EntityAdmin& entity_admin, const LayerType layer);

		~System();

	public:
		ArchetypeID GetIDKey() const override;
		const ComponentIDs& GetArchKey() const override;

		void SetPriority(const float val) override;

	public:
		/// Forces this system to run alone, ignoring all others in the layer
		/// 
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
		mutable ArchExclCache		m_excluded_archetypes;
		mutable ComponentIDs		m_arch_key;
	};

	inline auto ISystem::operator<=>(const ISystem& rhs) const
	{
		return GetPriority() <=> rhs.GetPriority();
	}

	inline float ISystem::GetPriority() const noexcept		{ return m_priority; }
	inline bool ISystem::IsRunningParallel() const noexcept { return m_run_parallel; }
	inline bool ISystem::IsEnabled() const noexcept			{ return m_enabled; }

	inline void ISystem::SetPriority(const float val)		{ m_priority = val; }
	inline void ISystem::SetRunParallel(const bool flag)	{ m_run_parallel = flag; }
	inline void ISystem::SetEnabled(const bool flag)		{ m_enabled = flag; }

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
	inline void System<Cs...>::SetPriority(const float val)
	{
		ISystem::SetPriority(val);
		m_entity_admin->SortSystems(m_layer);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::ForceRun()
	{
		m_entity_admin->RunSystem(this);
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
		m_exclusion = cu::Sort<ComponentIDs>({ ComponentTypeID(id::Type<Cs2>::ID())... });
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::Start() const
	{
		OnStart();
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::Run(const Archetype* const archetype) const
	{
		assert(IsEnabled());

		if (m_all_func || m_each_func) // check if func stores callable object
		{
			if (IsArchetypeExcluded(archetype)) // check that it is not excluded
				return;

			Run(archetype->type, 
				archetype->entities,
				archetype->component_data);
		}
	}

	template<class... Cs> requires IsComponents<Cs...>
	template<std::size_t Index, typename T, typename... Ts> requires (Index != sizeof...(Cs))
	inline void System<Cs...>::Run(const ComponentIDs& component_ids, std::span<const EntityID> entities, T& c, Ts... cs) const
	{
		using ComponentType = std::tuple_element_t<Index, ComponentTypes>; // get type of component at index in system components
		constexpr auto component_id = id::Type<ComponentType>::ID();

		std::size_t i = 0;
		ComponentTypeID archetype_comp_id = component_ids[i];
		while (archetype_comp_id != component_id && i < component_ids.size())	// iterate until matching component is found
		{
			archetype_comp_id = component_ids[++i];
		}

		assert(i != component_ids.size());

		Run<Index + 1>(component_ids, entities, c, cs..., reinterpret_cast<ComponentType*>(&c[i][0])); // run again on next component, or call final Run
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
		const auto ait = m_excluded_archetypes.find(archetype->id);
		if (ait != m_excluded_archetypes.end())
			return ait->excluded;

		bool excluded = false;
		for (const ComponentTypeID component_id : m_exclusion)
		{
			const auto it = cu::FindSorted(archetype->type, component_id);
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

