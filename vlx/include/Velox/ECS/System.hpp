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
#include <Velox/Config.hpp>

#include "Identifiers.hpp"
#include "Archetype.hpp"

namespace vlx
{
	class EntityAdmin;

	class SystemBase : private NonCopyable
	{
	public:
		virtual ~SystemBase() = default;

	public:
		auto operator<=>(const SystemBase& rhs) const;

	public:
		virtual ArchetypeID GetIDKey() const = 0;
		virtual const ComponentIDs& GetArchKey() const = 0;

		float GetPriority() const noexcept;
		bool IsRunningParallel() const noexcept;
		bool IsEnabled() const noexcept;

		virtual void SetPriority(float val);
		virtual void SetRunParallel(bool flag);
		virtual void SetEnabled(bool flag);

	public:
		virtual void Start() const;
		virtual void End() const;

	protected:
		virtual void Run(const Archetype* const archetype) const = 0;

	private:
		float	m_priority		{0.0f};		// priority is for controlling the underlying order of calls inside a layer
		bool	m_run_parallel	{false};	// determines if whether to parallelize archetypes when being run
		bool	m_enabled		{true};		// enables or disables system being run

		friend class EntityAdmin;
	};

	template<class... Cs> requires IsComponents<Cs...>
	class System : public SystemBase
	{
	public:
		using AllFunc			= std::function<void(std::span<const EntityID>, Cs*...)>;
		using EachFunc			= std::function<void(EntityID, Cs&...)>;
		using ComponentTypes	= std::tuple<Cs...>;

		static constexpr ArrComponentIDs<Cs...> SystemIDs =
			cu::Sort<ArrComponentIDs<Cs...>>({ id::Type<Cs>::ID()... }); // another stupid intellisense error

		static constexpr ArchetypeID SystemID =
			cu::ContainerHash<ComponentTypeID>()(SystemIDs);

	public:
		System(EntityAdmin& entity_admin);
		System(EntityAdmin& entity_admin, LayerType layer, bool add_to_layer = true);
		~System();

	public:
		const EntityAdmin* GetEntityAdmin() const noexcept;
		EntityAdmin* GetEntityAdmin() noexcept;

		ArchetypeID GetIDKey() const override;
		const ComponentIDs& GetArchKey() const override;

		void SetPriority(const float val) override;

	public:
		/// Forces this system to run alone, ignoring all others in the layer
		/// 
		void ForceRun();

		/// Forces this system to be added to the entity admin at specified layer
		/// 
		bool ForceAdd(LayerType layer);

		/// Forces this system to be removed from entity admin
		/// 
		bool ForceRemove();

		void All(AllFunc&& func);
		void Each(EachFunc&& func);

	protected:
		virtual void Run(const Archetype* const archetype) const override;

		template<std::size_t Index = 0, typename T, typename... Ts> requires (Index != sizeof...(Cs))
		void Run(const ComponentIDs& component_ids, std::span<const EntityID> entities, T& t, Ts... ts) const;

		template<std::size_t Index, typename T, typename... Ts> requires (Index == sizeof...(Cs))
		void Run(const ComponentIDs& component_ids, std::span<const EntityID> entities, T& t, Ts... ts) const;

	protected:	
		EntityAdmin*			m_entity_admin	{nullptr};
		LayerType				m_layer			{LYR_NONE};	// controls the overall order of calls
		bool					m_registered	{false};
		AllFunc					m_func;
	};

	template<class... Cs> requires IsComponents<Cs...>
	class SystemExclude final : public System<Cs...>
	{
	private:
		struct ArchExclData
		{
			ArchExclData(ArchetypeID id, bool excluded = false) : id(id), excluded(excluded) {};
			bool operator==(const ArchExclData& ae) const { return id == ae.id; }

			ArchetypeID id	{NULL_ARCHETYPE};
			bool excluded	{false};
		};

		using ArchExclCache = std::unordered_set<ArchExclData, decltype([](const ArchExclData& data) { return data.id; })>;

	public:
		using System<Cs...>::System;

	public:
		///	Exclude any entities that holds these components
		/// 
		template<class... Cs> requires IsComponents<Cs...>
		void Exclude();

	protected:
		void Run(const Archetype* const archetype) const override;

	private:
		bool IsArchetypeExcluded(const Archetype* archetype) const;

	protected:
		ComponentIDs			m_exclusion;
		mutable ArchExclCache	m_excluded_archetypes;
	};

	template<class... Cs> requires IsComponents<Cs...>
	class SystemEvent final : public System<Cs...>
	{
	public:
		using System<Cs...>::System;

	public:
		void Start() const override;
		void End() const override;

	public:
		Event<> OnStart;	// called before system starts being run for each archetype
		Event<> OnEnd;		// called after system has run for each archetype
	};

	// TODO: add way to include both functionality for events and exclusion

	inline auto SystemBase::operator<=>(const SystemBase& rhs) const	{ return GetPriority() <=> rhs.GetPriority(); }

	inline float SystemBase::GetPriority() const noexcept		{ return m_priority; }
	inline bool SystemBase::IsRunningParallel() const noexcept	{ return m_run_parallel; }
	inline bool SystemBase::IsEnabled() const noexcept			{ return m_enabled; }

	inline void SystemBase::SetPriority(float val)		{ m_priority = val; }
	inline void SystemBase::SetRunParallel(bool flag)	{ m_run_parallel = flag; }
	inline void SystemBase::SetEnabled(bool flag)		{ m_enabled = flag; }

	inline void SystemBase::Start() const	{}
	inline void SystemBase::End() const		{}

	template<class... Cs> requires IsComponents<Cs...>
	inline System<Cs...>::System(EntityAdmin& entity_admin) : m_entity_admin(&entity_admin)
	{

	}

	template<class... Cs> requires IsComponents<Cs...>
	inline System<Cs...>::System(EntityAdmin& entity_admin, LayerType layer, bool add_to_layer)
		: m_entity_admin(&entity_admin), m_layer(layer)
	{
		if (m_layer != LYR_NONE && add_to_layer)
		{
			m_entity_admin->RegisterSystem(m_layer, this);
			m_registered = true;
		}
	}


	template<class... Cs> requires IsComponents<Cs...>
	inline System<Cs...>::~System()
	{
		if (m_registered && m_entity_admin != nullptr)
			m_entity_admin->RemoveSystem(m_layer, this);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline const EntityAdmin* System<Cs...>::GetEntityAdmin() const noexcept
	{
		return m_entity_admin;
	}
	template<class... Cs> requires IsComponents<Cs...>
	inline EntityAdmin* System<Cs...>::GetEntityAdmin() noexcept
	{
		return m_entity_admin;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline ArchetypeID System<Cs...>::GetIDKey() const
	{
		return SystemID;
	}	

	template<class... Cs> requires IsComponents<Cs...>
	inline const ComponentIDs& System<Cs...>::GetArchKey() const
	{
		static ComponentIDs	arch_key = { SystemIDs.begin(), SystemIDs.end() };
		return arch_key;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::SetPriority(const float val)
	{
		SystemBase::SetPriority(val);
		m_entity_admin->SortSystems(m_layer);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::ForceRun()
	{
		m_entity_admin->RunSystem(this);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline bool System<Cs...>::ForceAdd(LayerType layer)
	{
		if (!m_registered && layer != LYR_NONE)
		{
			m_entity_admin->RegisterSystem(layer, this);

			m_layer = layer;
			m_registered = true;

			return true;
		}

		return false;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline bool System<Cs...>::ForceRemove()
	{
		if (m_registered && m_layer != LYR_NONE)
		{
			m_entity_admin->RemoveSystem(m_layer, this);

			m_layer = LYR_NONE;
			m_registered = false;

			return true;
		}

		return false;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::All(AllFunc&& func)
	{
		assert(!m_func);
		m_func = std::move(func);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::Each(EachFunc&& func)
	{
		assert(!m_func);
		m_func = [func = std::forward<EachFunc>(func)](std::span<const EntityID> entities, Cs*... cs)
		{
			for (std::size_t i = 0; i < entities.size(); ++i)
				func(entities[i], cs[i]...);
		};
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void System<Cs...>::Run(const Archetype* const archetype) const
	{
		assert(IsEnabled());

		if (m_func) // check if func stores callable object
		{
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
		if (m_func)
		{
			m_func(entities, ts...);
		}
	}

	template<class... Cs1> requires IsComponents<Cs1...>
	template<class... Cs2> requires IsComponents<Cs2...>
	inline void SystemExclude<Cs1...>::Exclude()
	{
		m_exclusion = cu::Sort<ComponentIDs>({ ComponentTypeID(id::Type<Cs2>::ID())... });
	}

	template<class ...Cs> requires IsComponents<Cs...>
	inline void SystemExclude<Cs...>::Run(const Archetype* const archetype) const
	{
		assert(this->IsEnabled());

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
	inline bool SystemExclude<Cs...>::IsArchetypeExcluded(const Archetype* archetype) const
	{
		const auto ait = m_excluded_archetypes.find(archetype->id);
		if (ait != m_excluded_archetypes.end())
			return ait->excluded;

		bool excluded = false;
		for (const ComponentTypeID component_id : m_exclusion)
		{
			const auto it = cu::FindSorted(archetype->type.begin(), archetype->type.end(), component_id);
			if (it != archetype->type.end() && *it == component_id) // if contains excluded component
			{
				excluded = true;
				break;
			}
		}

		m_excluded_archetypes.emplace(archetype->id, excluded);

		return excluded;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void SystemEvent<Cs...>::Start() const
	{
		OnStart();
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void SystemEvent<Cs...>::End() const
	{
		OnEnd();
	}
}

