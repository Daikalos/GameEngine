#pragma once

#include "System.hpp"

namespace vlx
{
	template<class S, class... Cs>
	class SystemOptional;

	template<class... Cs1, class... Cs2> requires IsComponents<Cs1...> && IsComponents<Cs2...> && (!Contains<Cs2, Cs1...> && ...)
	class SystemOptional<System<Cs1...>, Cs2...> final : public SystemBase
	{
	public:
		using AllFunc			= std::function<void(EntitySpan, Cs1*..., Cs2*...)>;

		using ArrComponentIDs	= ArrComponentIDs<std::remove_const_t<Cs1>...>;
		using ComponentTypes	= std::tuple<std::remove_const_t<Cs1>..., std::remove_const_t<Cs2>...>;

		static constexpr ArrComponentIDs SystemIDs =
			cu::Sort<ArrComponentIDs>({ id::Type<std::remove_const_t<Cs1>>::ID()... });

		static constexpr ArchetypeID SystemID =
			cu::ContainerHash<ComponentTypeID>{}(SystemIDs);

	public:
		SystemOptional() = delete;

		SystemOptional(EntityAdmin& entity_admin) 
			: m_entity_admin(&entity_admin) {};

		SystemOptional(EntityAdmin& entity_admin, LayerType layer, bool add_to_layer = true) 
			: m_entity_admin(&entity_admin), m_layer(layer)
		{
			if (m_layer != LYR_NONE && add_to_layer)
			{
				m_entity_admin->RegisterSystem(m_layer, this);
				m_registered = true;
			}
		}

		SystemOptional(SystemOptional&& rhs) noexcept 
			: m_entity_admin(rhs.m_entity_admin), m_layer(rhs.m_layer), m_registered(rhs.m_registered), m_func(std::move(rhs.m_func))
		{
			rhs.m_entity_admin	= nullptr;
			rhs.m_layer			= LYR_NONE;
			rhs.m_registered	= false;
		}

		~SystemOptional() 
		{
			if (m_registered && m_entity_admin != nullptr)
				m_entity_admin->RemoveSystem(m_layer, this);
		}

		auto operator=(SystemOptional&& rhs) noexcept -> SystemOptional&	
		{
			m_entity_admin		= rhs.m_entity_admin;
			m_layer				= rhs.m_layer;
			m_registered		= rhs.m_registered;
			m_func				= std::move(m_func);

			rhs.m_entity_admin	= nullptr;
			rhs.m_layer			= LYR_NONE;
			rhs.m_registered	= false;

			return *this;
		}

	public:
		NODISC const EntityAdmin* GetEntityAdmin() const noexcept	{ return m_entity_admin; }
		NODISC EntityAdmin* GetEntityAdmin() noexcept				{ return m_entity_admin; }

		NODISC virtual ArchetypeID GetIDKey() const override		{ return SystemID; }
		NODISC virtual ComponentIDSpan GetArchKey() const override	{ return SystemIDs; }

		/// Determines the priority of the system in the layer.
		/// 
		/// /param Value: priority value, for example, high value means that the system will likely be called first
		/// 
		virtual void SetPriority(float val) override 
		{
			SystemBase::SetPriority(val);
			m_entity_admin->SortSystems(m_layer);
		}

	public:
		/// Set function to be called for all the entities when system is run.
		/// 
		template<typename Func> requires HasParameters<Func, EntitySpan, Cs1*..., Cs2*...>
		void All(Func&& func) 
		{
			m_func = std::forward<Func>(func);
		}

		/// Set bare function to be called for all the entities when system is run.
		/// 
		template<typename Func> requires HasParameters<Func, std::size_t, Cs1*..., Cs2*...>
		void All(Func&& func) 
		{
			m_func = [func = std::forward<Func>(func)](EntitySpan entities, Cs1*... cs1, Cs2*... cs2)
			{
				func(entities.size(), cs1..., cs2...);
			};
		}

		/// Set member function to be called for all the entities when system is run.
		/// 
		template<class T, class U>
		void All(void(T::* f)(EntitySpan, Cs1*..., Cs2*...), U p) 
		{
			m_func = [f, p](EntitySpan entities, Cs1*... cs1, Cs2*... cs2)
			{
				(p->*f)(entities, cs1..., cs2...);
			};
		}

		/// Set const member function to be called for all the entities when system is run.
		/// 
		template<class T, class U>
		void All(void(T::* f)(EntitySpan, Cs1*..., Cs2*...) const, U p) 
		{
			m_func = [f, p](EntitySpan entities, Cs1*... cs1, Cs2*... cs2)
			{
				(p->*f)(entities, cs1..., cs2...);
			};
		}

		/// Set bare member function to be called for all the entities when system is run.
		/// 
		template<class T, class U>
		void All(void(T::* f)(std::size_t, Cs1*..., Cs2*...), U p) 
		{
			m_func = [f, p](EntitySpan entities, Cs1*... cs1, Cs2*... cs2)
			{
				(p->*f)(entities.size(), cs1..., cs2...);
			};
		}

		/// Set bare const member function to be called for all the entities when system is run.
		/// 
		template<class T, class U>
		void All(void(T::* f)(std::size_t, Cs1*..., Cs2*...) const, U p) 
		{
			m_func = [f, p](EntitySpan entities, Cs1*... cs1, Cs2*... cs2)
			{
				(p->*f)(entities.size(), cs1..., cs2...);
			};
		}

	public:
		/// Forces this system to run alone, ignoring all others in the layer.
		/// 
		void ForceRun() 
		{
			m_entity_admin->RunSystem(this);
		}

		/// Forces this system to be added to the entity admin at specified layer.
		/// 
		bool ForceAdd(LayerType layer) 
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

		/// Forces this system to be removed from entity admin.
		/// 
		bool ForceRemove() 
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

	protected:
		void Run(const Archetype* const archetype) const override 
		{
			assert(IsEnabled() && "System is disabled and cannot be run (EntityAdmin checks for this beforehand)");

			if (m_func) // check if func stores callable object
				RunImpl(archetype);
		}

		template<typename... Ts> requires (sizeof...(Ts) < (sizeof...(Cs1) + sizeof...(Cs2)))
		void RunImpl(const Archetype* const archetype, Ts... cs) const 
		{
			using ComponentType = std::tuple_element_t<sizeof...(Ts), ComponentTypes>; // get type of component at index in system components
			static constexpr auto find_id = EntityAdmin::GetComponentID<ComponentType>();

			auto i		 {0};
			auto curr_id {archetype->type[i]};

			while (curr_id != find_id && (i + 1) < archetype->type.size()) // iterate until matching component is found
			{
				curr_id = archetype->type[++i];
			}

			const bool found = (curr_id == find_id);

			// run again on next component, or call func
			if constexpr ((sizeof...(Ts) + 1) != (sizeof...(Cs1) + sizeof...(Cs2)))
			{
				RunImpl(archetype, cs..., found ? reinterpret_cast<ComponentType*>(&archetype->component_data[i][0]) : nullptr);
			}
			else
			{
				m_func(archetype->entities, cs..., found ? reinterpret_cast<ComponentType*>(&archetype->component_data[i][0]) : nullptr);
			}
		}

	protected:	
		EntityAdmin*	m_entity_admin	{nullptr};
		LayerType		m_layer			{LYR_NONE};	// controls the overall order of calls
		bool			m_registered	{false};
		AllFunc			m_func;
	};
}