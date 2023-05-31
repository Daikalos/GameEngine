#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <queue>
#include <execution>
#include <optional>
#include <cassert>

#include <Velox/System/Concepts.h>
#include <Velox/System/Event.hpp>
#include <Velox/VeloxTypes.hpp>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"
#include "Archetype.hpp"
#include "System.hpp"

namespace vlx
{
	// forward declarations

	struct IComponentAlloc;

	template<IsComponent>
	struct ComponentAlloc;

	template<class>
	class ComponentRef;

	template<class... Cs> requires IsComponents<Cs...>
	class ComponentSet;

	////////////////////////////////////////////////////////////
	// 
	// ECS based on article by Deckhead:
	// https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/ 
	// 
	////////////////////////////////////////////////////////////

	///	Data-oriented ECS design. Components are stored in contiguous memory inside of archetypes to maximize cache locality.
	/// 
	class EntityAdmin final : private NonCopyable
	{
	private:
		struct Record
		{
			Archetype*	archetype	{nullptr};
			IDType		index		{0}; // where in the archetype entity array is the entity located at
		};

		struct ArchetypeRecord
		{
			ColumnType	column		{0}; // where in the archetype is the components data located at
		};

		enum
		{
			CF_None			= 0,
			CF_Component	= 1 << 0,
			CF_Base			= 1 << 1,
			CF_All			= CF_Component | CF_Base
		};

		struct DataRef
		{
			struct
			{
				std::weak_ptr<void*> ptr;
			} component;

			struct
			{
				std::weak_ptr<void*> ptr;
				uint16 offset {0};
			} base;

			uint16 flag {0};
		};

		using ComponentPtr				= std::unique_ptr<IComponentAlloc>;
		using ArchetypePtr				= std::unique_ptr<Archetype>;

		using SystemsArrayMap			= std::unordered_map<LayerType, std::vector<ISystem*>>;
		using ArchetypesArray			= std::vector<ArchetypePtr>;
		using ArchetypeMap				= std::unordered_map<ArchetypeID, Archetype*>;
		using EntityArchetypeMap		= std::unordered_map<EntityID, Record>;
		using EntityComponentRefMap		= std::unordered_map<EntityID, std::unordered_map<ComponentTypeID, DataRef>>;
		using ComponentTypeIDBaseMap	= std::unordered_map<ComponentTypeID, ComponentPtr>;
		using ComponentArchetypesMap	= std::unordered_map<ComponentTypeID, std::unordered_map<ArchetypeID, ArchetypeRecord>>;
		using ArchetypeCache			= std::unordered_map<ArchetypeID, std::vector<Archetype*>>;
		using EventMap					= std::unordered_map<ComponentTypeID, Event<EntityID, void*>>;

		template<IsComponent>
		friend struct ComponentAlloc;

	public:
		EntityAdmin() = default;
		VELOX_API ~EntityAdmin();

	public: 
		/// \returns The ID of the component
		///
		template<IsComponent C>
		NODISC static constexpr ComponentTypeID GetComponentID();

		/// Register the component for later usage. Has to be done before the component can be used in the ECS.
		///
		template<IsComponent C>
		void RegisterComponent();

		///	Shortcut for registering multiple components.
		///
		template<class... Cs> requires IsComponents<Cs...>
		void RegisterComponents();

		///	Shortcut for registering multiple components.
		///
		template<class... Cs> requires IsComponents<Cs...>
		void RegisterComponents(std::type_identity<std::tuple<Cs...>>);

		///	Adds a component to the specified entity.
		/// 
		/// \param EntityID: ID of the entity to add the component to.
		/// \param Args: Optional constructor aruguments.
		///	
		/// \returns Pointer to the added component if it was successful, otherwise nullptr.
		///
		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		C* AddComponent(EntityID entity_id, Args&&... args);

		///	Optimized for quickly adding multiple components to an entity.
		/// 
		/// \param EntityID: ID of the entity to add the component to.
		///
		template<class... Cs> requires IsComponents<Cs...>
		void AddComponents(EntityID entity_id);

		///	Optimized for quickly adding multiple components to an entity.
		/// 
		/// \param EntityID: ID of the entity to add the component to.
		/// \param Tuple: To automatically deduce the template arguments.
		/// 
		template<class... Cs> requires IsComponents<Cs...>
		void AddComponents(EntityID entity_id, std::type_identity<std::tuple<Cs...>>);

		/// Removes a component from the specified entity. Will return true if it succeeded in doing such, otherwise false.
		/// 
		/// \param EntityID: ID of the entity to remove the component from.
		/// 
		/// \returns True if it succeeded in removing component, otherwise false
		/// 
		template<IsComponent C>
		bool RemoveComponent(EntityID entity_id);

		/// Optimized for quickly removing multiple components to an entity. If the entity does not hold a given component, it will be skipped.
		///	
		/// \param EntityID: ID of the entity to remove the components from.
		/// 
		/// \returns Whether if it was able to remove any component from the entity.
		/// 
		template<class... Cs> requires IsComponents<Cs...>
		bool RemoveComponents(EntityID entity_id);

		///	Optimized for quickly removing multiple components to an entity. If the entity does not hold a given component, it will be skipped.
		///	
		/// \param EntityID: ID of the entity to remove the components from.
		/// \param Tuple: To automatically deduce the template arguments.
		/// 
		/// \returns Whether if it was able to remove any component from the entity.
		/// 
		template<class... Cs> requires IsComponents<Cs...>
		bool RemoveComponents(EntityID entity_id, std::type_identity<std::tuple<Cs...>>);

		///	GetComponent is designed to be as fast as possible without checks to see if it exists, otherwise, will throw error. 
		/// Therefore, take some caution when using this function. Use instead: TryGetComponent or GetComponentRef for better safety.
		/// 
		/// \param EntityID: ID of the entity to retrieve the component from.
		/// 
		/// \returns Reference to component.
		/// 
		template<IsComponent C>
		NODISC C& GetComponent(EntityID entity_id) const;

		/// Tries to get the component. May fail if the entity does not exist or hold the component.
		/// 
		/// \param EntityID: ID of the entity to retrieve the component from.
		/// 
		/// \returns Pointer to component, otherwise std::nullopt.
		/// 
		template<IsComponent C>
		NODISC std::optional<C*> TryGetComponent(EntityID entity_id) const;

		/// Retrieves multiple components from an entity in one go.
		/// 
		/// \param EntityID: ID of the entity to retrieve the components from.
		/// 
		/// \returns Tuple containing pointers to component
		/// 
		template<class... Cs> requires (IsComponents<Cs...> && sizeof...(Cs) > 1)
		NODISC std::tuple<Cs*...> GetComponents(EntityID entity_id) const;

		/// Defaults to GetComponent if GetComponents only specifies one type.
		/// 
		template<class... Cs> requires (IsComponents<Cs...> && sizeof...(Cs) == 1)
		NODISC std::tuple_element_t<0, std::tuple<Cs...>>& GetComponents(EntityID entity_id) const;

		///	Constructs a ComponentSet that contains a set of component references that ensures that they remain valid.
		///
		/// \param EntityID: ID of the entity to retrieve the components from.
		/// 
		/// \returns A ComponentSet constructed from the specified components.
		/// 
		template<class... Cs> requires IsComponents<Cs...>
		NODISC ComponentSet<Cs...> GetComponentsRef(EntityID entity_id) const;

		///	Allows for you to retrieve any base class without having to know the type of the child.
		/// 
		/// [Incredibly risky, requires base to be first in inheritance, other base classes cannot be automatically 
		/// found without using voodoo magic, for now, offset can be specified to find the correct base class in the 
		/// inheritance order, for example, "class Component : B, A", to find A you pass offset with sizeof(B)]
		/// 
		/// \param EntityID: ID of the entity to retrieve the child from.
		/// \param ChildComponentID: ID of the child component to upcast to base.
		/// \param Offset: Offset in bytes to specify the location of base in the inheritance order.
		/// 
		/// \returns Reference to base.
		/// 
		template<class B>
		NODISC B& GetBase(EntityID entity_id, ComponentTypeID child_component_id, const std::size_t offset = 0) const;

		/// Tries to get the base. May fail if entity does not exist or hold the child component.
		/// 
		/// \param EntityID: ID of the entity to retrieve the child from.
		/// \param ChildComponentID: ID of the child component to upcast to base.
		/// \param Offset: Offset in bytes to specify the location of base in the inheritance order.
		/// 
		/// \returns Pointer to base, otherwise std::nullopt.
		/// 
		template<class B>
		NODISC std::optional<B*> TryGetBase(EntityID entity_id, ComponentTypeID child_component_id, const std::size_t offset = 0) const;

		/// Modifies the existing component with a newly constructed one.
		/// 
		/// \param EntityID: ID of the entity to retrieve the component from.
		/// \param Args: Optional constructor parameters.
		/// 
		/// \returns Reference to the component that was modified
		/// 
		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		C& SetComponent(EntityID entity_id, Args&&... args);

		/// Tries to modify the existing component with a newly constructed one. May fail if the entity does 
		/// not exist or hold the component.
		///
		/// \param EntityID: ID of the entity to retrieve the component from.
		/// \param Args: Optional constructor parameters.
		/// 
		/// \returns Pointer to the component that was modified, otherwise std::nullopt
		/// 
		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		std::optional<C*> TrySetComponent(EntityID entity_id, Args&&... args);

		///	Returns a reference for the component whose pointer will remain valid even when the archetype is modified.
		/// 
		///	\param EntityID: ID of the entity to retrieve the component from.
		/// \param Component: Pointer to component, leave nullptr to automatically retrieve.
		/// 
		/// \returns A component reference
		/// 
		template<IsComponent C>
		NODISC ComponentRef<C> GetComponentRef(EntityID entity_id, C* component = nullptr) const;

		///	Tries to return a component ref. May fail if the entity does not exist or hold the specified component.
		/// 
		///	\param EntityID: ID of the entity to retrieve the component from.
		/// \param Component: Pointer to component, leave nullptr to automatically retrieve.
		/// 
		/// \returns A component reference if succesful, will return std::nullopt otherwise.
		/// 
		template<IsComponent C>
		NODISC std::optional<ComponentRef<C>> TryGetComponentRef(EntityID entity_id, C* component = nullptr) const;

		///	Returns a reference for the base whose pointer will remain valid even when the archetype is modified. 
		/// 
		///	\param EntityID: ID of the entity to retrieve the base from.
		/// \param ChildComponentID: ID of the child component to upcast to base.
		/// \param Offset: Offset in bytes to specify the location of base in the inheritance order.
		/// \param Base: Pointer to base, leave nullptr to automatically retrieve.
		/// 
		/// \returns A base reference if succesful, will return std::nullopt otherwise.
		/// 
		template<class B>
		NODISC ComponentRef<B> GetBaseRef(EntityID entity_id, ComponentTypeID child_component_id, const uint32 offset = 0, B* base = nullptr) const;

		///	Tries to return a base reference. May fail if the entity does not exist or hold the specified component.
		/// 
		///	\param EntityID: ID of the entity to retrieve the base from.
		/// \param ChildComponentID: ID of the child component to upcast to base.
		/// \param Offset: Offset in bytes to specify the location of base in the inheritance order.
		/// \param Base: Pointer to base, leave nullptr to automatically retrieve.
		/// 
		/// \returns A base reference if succesful, will return std::nullopt otherwise.
		/// 
		template<class B>
		NODISC std::optional<ComponentRef<B>> TryGetBaseRef(EntityID entity_id, ComponentTypeID child_component_id, const uint32 offset = 0, B* base = nullptr) const;

		///	Checks if the entity holds the specified component.
		/// 
		/// \param EntityID: ID of the entity to check.
		/// 
		/// \returns True if the entity contains the component C, otherwise false.
		/// 
		template<IsComponent C>
		NODISC bool HasComponent(EntityID entity_id) const;

		///	Checks if the component is registered
		/// 
		///	\returns True if the component has been registered in the ECS, otherwise false.
		/// 
		template<IsComponent C>
		NODISC bool IsComponentRegistered() const;

	public:
		///	Sorts the components for all entities that exactly contains the specified components. The components 
		/// is sorted according to the comparison function. Do note that it will also sort all other components 
		/// the entities may contain to maintain order.
		/// 
		/// \param Comparison: Comparison function between two components that will determine the order.
		/// 
		/// \returns True if sorting was succesful, otherwise false
		/// 
		template<class... Cs, class Comp> requires IsComponents<Cs...>
		bool SortComponents(Comp&& comparison) requires SameTypeParamDecay<Comp, std::tuple_element_t<0, std::tuple<Cs...>>, 0, 1>;

		///	Sorts the components for a specific entity, will also sort the components for all other entities
		///	that holds the same components as the specified entity. Will also sort all other components that 
		///	the entities may contain.
		/// 
		/// \param EntityID: ID of the entity to sort by.
		/// \param Comparison: Comparison function between two components that will determine the order.
		/// 
		/// \returns True if sorting was succesful, otherwise false
		/// 
		template<IsComponent C, class Comp> requires SameTypeParamDecay<Comp, C, 0, 1>
		bool SortComponents(EntityID entity_id, Comp&& comparison);

		/// Searches for entities that contains the specified components
		/// 
		/// \param Restricted: Returns all entities that exactly match the provided components
		/// 
		/// \returns Entities that contains the components
		/// 
		template<class... Cs> requires IsComponents<Cs...>
		NODISC std::vector<EntityID> GetEntitiesWith(bool restricted = false) const;

		///	Increases the capacity of the archetype containing an exact match of the specified components.
		/// 
		/// \param ComponentCount: Number of components to reserve for in the archetypes
		/// 
		template<class... Cs> requires IsComponents<Cs...>
		void Reserve(const std::size_t component_count);

		template<IsComponent C, typename Func>
		NODISC auto RegisterOnAddListener(Func&& func);

		template<IsComponent C, typename Func>
		NODISC auto RegisterOnMoveListener(Func&& func);

		template<IsComponent C, typename Func>
		NODISC auto RegisterOnRemoveListener(Func&& func);

		template<IsComponent C>
		void DeregisterOnAddListener(const typename EventHandler<>::IDType id);

		template<IsComponent C>
		void DeregisterOnMoveListener(const typename EventHandler<>::IDType id);

		template<IsComponent C>
		void DeregisterOnRemoveListener(const typename EventHandler<>::IDType id);

	public:
		///	Returns a duplicated entity with the same properties as the specified one
		/// 
		/// \param EntityID: entity id of the one to copy the components from
		/// 
		/// \returns ID of the newly created entity containing the copied components
		///
		VELOX_API NODISC EntityID Duplicate(EntityID entity_id);

		///	Shrinks the ECS by removing all the empty archetypes.
		/// 
		/// \param Extensive: Perform a complete shrink of the ECS by removing all the extra data space.
		/// 
		VELOX_API void Shrink(bool extensive = false);

	public:
		template<IsContainer T>
		NODISC std::vector<EntityID> GetEntitiesWith(const T& component_ids, const ArchetypeID archetype_id, bool restricted = false) const;

		template<IsContainer T>
		void Reserve(const T& component_ids, const ArchetypeID archetype_id, const std::size_t component_count);

	private:
		template<IsContainer T>
		NODISC Archetype* GetArchetype(const T& component_ids, const ArchetypeID archetype_id);
		template<IsContainer T>
		Archetype* CreateArchetype(const T& component_ids, const ArchetypeID archetype_id);

		template<IsContainer T>
		NODISC std::span<const Archetype* const> GetArchetypes(const T& component_ids, const ArchetypeID archetype_id) const;

		template<IsComponent C, class Comp>
		bool SortComponents(Archetype* archetype, Comp&& comp);

		template<IsComponent C>
		void EraseComponentRef(EntityID entity_id) const;

		template<IsComponent C>
		void UpdateComponentRef(EntityID entity_id, C* new_component) const;

	public:
		VELOX_API NODISC EntityID GetNewEntityID();

		VELOX_API NODISC bool IsEntityRegistered(EntityID entity_id) const;
		VELOX_API NODISC bool HasComponent(EntityID entity_id, ComponentTypeID component_id) const;

		VELOX_API auto RegisterEntity(EntityID entity_id) -> Record&;
		VELOX_API void RegisterSystem(LayerType layer, ISystem* system);

		VELOX_API void RemoveSystem(LayerType layer, ISystem* system);
		VELOX_API bool RemoveEntity(EntityID entity_id);

		VELOX_API void RunSystems(LayerType layer) const;
		VELOX_API void SortSystems(LayerType layer);

		VELOX_API void RunSystem(const ISystem* system) const;

		VELOX_API void AddComponent(EntityID entity_id, ComponentTypeID add_component_id);
		VELOX_API bool RemoveComponent(EntityID entity_id, ComponentTypeID rmv_component_id);

		VELOX_API void AddComponents(EntityID entity_id, const ComponentIDs& component_ids, ArchetypeID archetype_id);
		VELOX_API bool RemoveComponents(EntityID entity_id, const ComponentIDs& component_ids, ArchetypeID archetype_id);

		VELOX_API void DeregisterOnAddListener(ComponentTypeID component_id, typename EventHandler<>::IDType id);
		VELOX_API void DeregisterOnMoveListener(ComponentTypeID component_id, typename EventHandler<>::IDType id);
		VELOX_API void DeregisterOnRemoveListener(ComponentTypeID component_id, typename EventHandler<>::IDType id);

		VELOX_API NODISC bool HasShutdown() const;
		VELOX_API void Shutdown();

	private:
		VELOX_API void CallOnAddEvent(ComponentTypeID component_id, EntityID eid, void* data) const;
		VELOX_API void CallOnMoveEvent(ComponentTypeID component_id, EntityID eid, void* data) const;
		VELOX_API void CallOnRemoveEvent(ComponentTypeID component_id, EntityID eid, void* data) const;

		VELOX_API void EraseComponentRef(EntityID entity_id, ComponentTypeID component_id) const;
		VELOX_API void UpdateComponentRef(EntityID entity_id, ComponentTypeID component_id, void* new_component) const;

		VELOX_API void ClearEmptyEntityArchetypes();
		VELOX_API void ClearEmptyTypeArchetypes();

		VELOX_API void ConstructSwap(Archetype* new_archetype, Archetype* old_archetype, EntityID entity_id, const Record& record, EntityID last_entity_id, Record& last_record) const;
		VELOX_API void Construct(Archetype* new_archetype, Archetype* old_archetype, EntityID entity_id, const Record& record) const;

		VELOX_API void DestructSwap(Archetype* old_archetype, Archetype* new_archetype, EntityID entity_id, const Record& record, EntityID last_entity_id, Record& last_record) const;
		VELOX_API void Destruct(Archetype* old_archetype, Archetype* new_archetype, EntityID entity_id, const Record& record) const;

		VELOX_API void MakeRoom(Archetype* archetype, const IComponentAlloc* component, const std::size_t data_size, const std::size_t i) const;

	private:
		EntityID				m_entity_id_counter {1};	// current id counter for entities
		std::queue<EntityID>	m_reusable_entity_ids;		// reusable ids of entities that have been destroyed

		SystemsArrayMap			m_systems;						// map layer to array of systems (layer allows for controlling the order of calls)
		ArchetypesArray			m_archetypes;					// find matching archetype to update matching entities
		ArchetypeMap			m_archetype_map;				// map set of components to matching archetype that contains such components
		EntityArchetypeMap		m_entity_archetype_map;			// map entity to where its data is located at in the archetype
		ComponentArchetypesMap	m_component_archetypes_map;		// map component to the archetypes it exists in and where all of the components data in the archetype is located at
		ComponentTypeIDBaseMap	m_component_map;				// access to helper functions for modifying each unique component

		EventMap				m_events_add;
		EventMap				m_events_move;
		EventMap				m_events_remove;
		
		mutable ArchetypeCache			m_archetype_cache;
		mutable EntityComponentRefMap	m_entity_component_ref_map;

		bool m_shutdown {false};
	};
}

#include "ComponentAlloc.hpp"

namespace vlx
{
	template<IsComponent C>
	inline static constexpr ComponentTypeID EntityAdmin::GetComponentID()
	{
		return ComponentAlloc<C>::GetTypeID();
	}

	template<IsComponent C>
	inline void EntityAdmin::RegisterComponent()
	{
		auto insert = m_component_map.try_emplace(GetComponentID<C>(), std::make_unique<ComponentAlloc<C>>());
		assert(insert.second);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void EntityAdmin::RegisterComponents()
	{
		(RegisterComponent<Cs>(), ...);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void EntityAdmin::RegisterComponents(std::type_identity<std::tuple<Cs...>>)
	{
		RegisterComponents<Cs...>();
	}

	template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
	inline C* EntityAdmin::AddComponent(EntityID entity_id, Args&&... args)
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		const auto eit = m_entity_archetype_map.find(entity_id);
		if (eit == m_entity_archetype_map.end())
			return nullptr;

		Record& record = eit->second;
		Archetype* old_archetype = record.archetype;

		C* add_component = nullptr;
		Archetype* new_archetype = nullptr; // we are going to be moving to a new archetype

		constexpr ComponentTypeID add_component_id = GetComponentID<C>();

		if (old_archetype) // already has an attached archetype, define a new archetype
		{
			const auto ait = old_archetype->edges.find(add_component_id);
			if (ait == old_archetype->edges.end())
			{
				ComponentIDs new_archetype_id = old_archetype->type; // create copy
				if (!cu::InsertSorted(new_archetype_id, add_component_id)) // insert while keeping the vector sorted
					return nullptr;

				new_archetype = GetArchetype(new_archetype_id, cu::ContainerHash<ComponentIDs>()(new_archetype_id));

				old_archetype->edges[add_component_id].add = new_archetype;
				new_archetype->edges[add_component_id].rmv = old_archetype;

				assert(new_archetype_id != old_archetype->type);
				assert(new_archetype_id == new_archetype->type);
			}
			else new_archetype = ait->second.add;

			if (!new_archetype) // exit if no archetype was found
				return nullptr;

			EntityID last_entity_id = old_archetype->entities.back();
			assert(last_entity_id != NULL_ENTITY);

			if (last_entity_id != entity_id) // not same, we'll swap last to current for faster adding
			{
				Record& last_record = m_entity_archetype_map[last_entity_id];

				for (std::size_t i = 0, j = 0; i < new_archetype->type.size(); ++i) // move all the data from old to new and perform swaps at the same time
				{
					const auto component_id		= new_archetype->type[i];
					const auto component		= m_component_map[component_id].get();
					const auto component_size	= component->GetSize();

					const auto current_size		= new_archetype->entities.size() * component_size;
					const auto new_size			= current_size + component_size;

					if (new_size > new_archetype->component_data_size[i])
						MakeRoom(new_archetype, component, component_size, i);

					if (component_id == add_component_id)
					{
						assert(add_component == nullptr); // should never happen twice

						add_component = new(&new_archetype->component_data[i][current_size])
							C(std::forward<Args>(args)...);
					}
					else
					{
						component->MoveDestroyData(*this, entity_id,
							&old_archetype->component_data[j][record.index * component_size],
							&new_archetype->component_data[i][current_size]);

						component->MoveDestroyData(*this, last_entity_id,
							&old_archetype->component_data[j][last_record.index * component_size],
							&old_archetype->component_data[j][record.index * component_size]); // move data from last to current

						++j;
					}
				}

				old_archetype->entities[record.index] = old_archetype->entities.back();
				last_record.index = record.index;
			}
			else // same, usually means that this entity is at the back, just perform normal moving
			{
				for (std::size_t i = 0, j = 0; i < new_archetype->type.size(); ++i) // move all the data from old to new and perform swaps at the same time
				{
					const auto component_id		= new_archetype->type[i];
					const auto component		= m_component_map[component_id].get();
					const auto component_size	= component->GetSize();

					const auto current_size		= new_archetype->entities.size() * component_size;
					const auto new_size			= current_size + component_size;

					if (new_size > new_archetype->component_data_size[i])
						MakeRoom(new_archetype, component, component_size, i);

					if (component_id == add_component_id)
					{
						assert(add_component == nullptr); // should never happen twice

						add_component = new(&new_archetype->component_data[i][current_size])
							C(std::forward<Args>(args)...);
					}
					else
					{
						component->MoveDestroyData(*this, entity_id,
							&old_archetype->component_data[j][record.index * component_size],
							&new_archetype->component_data[i][current_size]);

						++j;
					}
				}
			}

			assert(add_component != nullptr); // a new component should have been added

			old_archetype->entities.pop_back(); // by only removing the last entity, it means that when the next component is added, it will overwrite the previous
		}
		else // if the entity has no archetype, first component
		{
			ComponentIDs new_archetype_id(1, add_component_id);	// construct archetype with the component id
			new_archetype = GetArchetype(new_archetype_id, cu::ContainerHash<ComponentIDs>()(new_archetype_id)); // construct or get archetype using the id

			const auto component		= m_component_map[add_component_id].get();
			const auto component_size	= component->GetSize();

			const auto current_size	= new_archetype->entities.size() * component_size;
			const auto new_size		= current_size + component_size;

			if (new_size > new_archetype->component_data_size[0])
				MakeRoom(new_archetype, component, component_size, 0); // make room and move over existing data

			add_component = new(&new_archetype->component_data[0][current_size])
				C(std::forward<Args>(args)...);
		}

		if constexpr (HasEvent<C, CreatedEvent>)
			add_component->Created(*this, entity_id);

		CallOnAddEvent(add_component_id, entity_id, static_cast<void*>(add_component));

		new_archetype->entities.push_back(entity_id);
		record.index = IDType(new_archetype->entities.size() - 1);
		record.archetype = new_archetype;

		return add_component;
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void EntityAdmin::AddComponents(EntityID entity_id)
	{
		constexpr auto component_ids = cu::Sort<ArrComponentIDs<Cs...>>({ GetComponentID<Cs>()... });
		constexpr auto archetype_id = cu::ContainerHash<ArrComponentIDs<Cs...>>()(component_ids);

		AddComponents(entity_id, { component_ids.begin(), component_ids.end() }, archetype_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void EntityAdmin::AddComponents(EntityID entity_id, std::type_identity<std::tuple<Cs...>>)
	{
		AddComponents<Cs...>(entity_id);
	}

	template<IsComponent C>
	inline bool EntityAdmin::RemoveComponent(EntityID entity_id)
	{
		return RemoveComponent(entity_id, GetComponentID<C>());
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline bool EntityAdmin::RemoveComponents(EntityID entity_id)
	{
		constexpr auto component_ids = cu::Sort<ArrComponentIDs<Cs...>>({ GetComponentID<Cs>()... });
		constexpr auto archetype_id = cu::ContainerHash<ArrComponentIDs<Cs...>>()(component_ids);

		return RemoveComponents(entity_id, { component_ids.begin(), component_ids.end() }, archetype_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline bool EntityAdmin::RemoveComponents(EntityID entity_id, std::type_identity<std::tuple<Cs...>>)
	{
		return RemoveComponents<Cs...>(entity_id);
	}

	template<IsComponent C>
	inline C& EntityAdmin::GetComponent(EntityID entity_id) const
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		constexpr ComponentTypeID component_id = GetComponentID<C>();

		const auto& record = m_entity_archetype_map.at(entity_id);
		const auto* archetype = record.archetype;

		const auto& map = m_component_archetypes_map.at(component_id);
		const auto& arch_record = map.at(archetype->id);
		
		C* components = reinterpret_cast<C*>(&archetype->component_data[arch_record.column][0]);
		return components[record.index];
	}

	template<IsComponent C>
	inline std::optional<C*> EntityAdmin::TryGetComponent(EntityID entity_id) const
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		const auto eit = m_entity_archetype_map.find(entity_id);
		if (eit == m_entity_archetype_map.end())
			return std::nullopt;

		const auto& record		= eit->second;
		const auto* archetype	= record.archetype;

		if (archetype == nullptr)
			return std::nullopt;

		constexpr ComponentTypeID component_id = GetComponentID<C>();

		const auto cit = m_component_archetypes_map.find(component_id);
		if (cit == m_component_archetypes_map.end())
			return std::nullopt;

		const auto ait = cit->second.find(archetype->id);
		if (ait == cit->second.end())
			return std::nullopt;

		const auto& arch_record = ait->second;

		C* components = reinterpret_cast<C*>(&archetype->component_data[arch_record.column][0]);
		return &components[record.index];
	}

	template<class B>
	inline B& EntityAdmin::GetBase(EntityID entity_id, ComponentTypeID child_component_id, const std::size_t offset) const
	{
		const auto& record = m_entity_archetype_map.at(entity_id);
		const auto* archetype = record.archetype;

		const auto& map = m_component_archetypes_map.at(child_component_id);
		const auto& arch_record = map.at(archetype->id);

		const auto component = m_component_map.at(child_component_id).get();
		const auto component_size = component->GetSize();

		auto ptr = &archetype->component_data[arch_record.column][record.index * component_size];
		B* base_component = reinterpret_cast<B*>(ptr + offset);	

		return *base_component;
	}

	template<class B>
	inline std::optional<B*> EntityAdmin::TryGetBase(EntityID entity_id, ComponentTypeID child_component_id, const std::size_t offset) const
	{
		const auto eit = m_entity_archetype_map.find(entity_id);
		if (eit == m_entity_archetype_map.end())
			return std::nullopt;

		const auto& record = eit->second;
		const auto* archetype = record.archetype;

		if (archetype == nullptr)
			return std::nullopt;

		const auto cit = m_component_archetypes_map.find(child_component_id);
		if (cit == m_component_archetypes_map.end())
			return std::nullopt;

		const auto ait = cit->second.find(archetype->id);
		if (ait == cit->second.end())
			return std::nullopt;

		const auto iit = m_component_map.find(child_component_id);
		if (iit == m_component_map.end())
			return std::nullopt;

		const auto component_size = iit->second->GetSize();

		auto ptr = &archetype->component_data[ait->second.column][record.index * component_size];
		B* base_component = reinterpret_cast<B*>(ptr + offset);

		return base_component;
	}

	template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
	inline C& EntityAdmin::SetComponent(EntityID entity_id, Args&&... args)
	{
		C& old_component = GetComponent<C>(entity_id);
		C new_component(std::forward<Args>(args)...);

		if constexpr (HasEvent<C, AlteredEvent>)
			old_component.Altered(*this, entity_id, new_component);

		old_component = std::move(new_component);

		return old_component;
	}

	template<IsComponent C, typename ...Args> requires std::constructible_from<C, Args...>
	inline std::optional<C*> EntityAdmin::TrySetComponent(EntityID entity_id, Args&&... args)
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		const auto opt_component = TryGetComponent<C>(entity_id);

		if (!opt_component)
			return std::nullopt;

		C* old_component = opt_component.value();
		C new_component(std::forward<Args>(args)...);

		if constexpr (HasEvent<C, AlteredEvent>)
			old_component->Altered(*this, entity_id, new_component);

		*old_component = std::move(new_component);

		return old_component;
	}

	template<IsComponent C>
	inline ComponentRef<C> EntityAdmin::GetComponentRef(EntityID entity_id, C* component) const
	{
		assert(IsComponentRegistered<C>());

		auto& component_refs = m_entity_component_ref_map[entity_id]; // will construct new if it does not exist
		constexpr ComponentTypeID component_id = GetComponentID<C>();

		const auto cit = component_refs.find(component_id);
		if (cit == component_refs.end()) // it does not yet exist
		{
			if (component == nullptr)
				component = &GetComponent<C>(entity_id);

			auto ptr = std::make_shared<void*>(component);

			DataRef data {ptr, {}, CF_Component};
			component_refs.try_emplace(component_id, data);

			return ComponentRef<C>(entity_id, ptr);
		}

		DataRef& ref = cit->second;
		if ((ref.flag & CF_Component) == CF_Component && ref.component.ptr.expired())
		{
			if (component == nullptr)
				component = &GetComponent<C>(entity_id);

			auto ptr = std::make_shared<void*>(component);
			ref.component.ptr = ptr;

			return ComponentRef<C>(entity_id, ptr);
		}

		return ComponentRef<C>(entity_id, ref.component.ptr.lock());
	}

	template<IsComponent C>
	inline std::optional<ComponentRef<C>> EntityAdmin::TryGetComponentRef(EntityID entity_id, C* component) const
	{
		if (!IsEntityRegistered(entity_id) || !HasComponent<C>(entity_id))
			return std::nullopt;

		return GetComponentRef<C>(entity_id, component);
	}

	template<class B>
	inline ComponentRef<B> EntityAdmin::GetBaseRef(EntityID entity_id, ComponentTypeID child_component_id, const uint32 offset, B* base) const
	{
		auto& component_refs = m_entity_component_ref_map[entity_id]; // will construct new if it does not exist

		const auto cit = component_refs.find(child_component_id);
		if (cit == component_refs.end()) // it does not yet exist
		{
			if (base == nullptr)
				base = &GetBase<B>(entity_id, child_component_id, offset);

			auto ptr = std::make_shared<void*>(base);

			DataRef data {{}, ptr, offset, CF_Base};
			component_refs.try_emplace(child_component_id, data);

			return ComponentRef<B>(entity_id, ptr);
		}

		DataRef& ref = cit->second;
		if ((ref.flag & CF_Base) == CF_Base && ref.base.ptr.expired())
		{
			if (base == nullptr)
				base = &GetBase<B>(entity_id, child_component_id, offset);

			auto ptr = std::make_shared<void*>(base);
			ref.base.ptr = ptr;

			return ComponentRef<B>(entity_id, ptr);
		}

		return ComponentRef<B>(entity_id, ref.base.ptr.lock());
	}

	template<class B>
	inline std::optional<ComponentRef<B>> EntityAdmin::TryGetBaseRef(EntityID entity_id, ComponentTypeID child_component_id, const uint32 offset, B* base) const
	{
		if (!IsEntityRegistered(entity_id) || !HasComponent(entity_id, child_component_id)) // check if entity exists and has component
			return std::nullopt;

		return GetBaseRef<B>(entity_id, child_component_id, offset, base);
	}

	template<IsComponent C>
	inline bool EntityAdmin::HasComponent(EntityID entity_id) const
	{
		assert(IsComponentRegistered<C>()); // component should be registered
		return HasComponent(entity_id, GetComponentID<C>());
	}

	template<IsComponent C>
	inline bool EntityAdmin::IsComponentRegistered() const
	{
		return m_component_map.contains(GetComponentID<C>());
	}

	template<class... Cs> requires (IsComponents<Cs...> && sizeof...(Cs) > 1)
	inline std::tuple<Cs*...> EntityAdmin::GetComponents(EntityID entity_id) const
	{
		using ComponentTypes = std::tuple<Cs...>;

		std::tuple<Cs*...> result;

		const auto& record = m_entity_archetype_map.at(entity_id);

		(([this, &result, &record]<class C, std::size_t N>() -> void
		{
			constexpr ComponentTypeID component_id = GetComponentID<C>();

			const auto& map = m_component_archetypes_map.at(component_id);
			const auto& arch_record = map.at(record.archetype->id);

			C* components = reinterpret_cast<C*>(&record.archetype->component_data[arch_record.column][0]);
			std::get<N>(result) = &components[record.index];
		}.operator()<Cs, traits::IndexInTuple<Cs, ComponentTypes>::value>()), ...);

		return result;
	}

	template<class... Cs> requires (IsComponents<Cs...> && sizeof...(Cs) == 1)
	inline std::tuple_element_t<0, std::tuple<Cs...>>& EntityAdmin::GetComponents(EntityID entity_id) const
	{
		return GetComponent<Cs...>(entity_id);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline ComponentSet<Cs...> EntityAdmin::GetComponentsRef(EntityID entity_id) const
	{
		return ComponentSet<Cs...>(GetComponentRef<Cs>(entity_id)...);
	}

	template<class... Cs, class Comp> requires IsComponents<Cs...>
	inline bool EntityAdmin::SortComponents(Comp&& comparison) requires SameTypeParamDecay<Comp, std::tuple_element_t<0, std::tuple<Cs...>>, 0, 1>
	{
		using C = std::tuple_element_t<0, std::tuple<Cs...>>; // the component that is meant to be sorted

		constexpr auto component_ids	= cu::Sort<ArrComponentIDs<Cs...>>({ GetComponentID<Cs>()... });
		constexpr auto archetype_id		= cu::ContainerHash<ArrComponentIDs<Cs...>>()(component_ids);

		const auto it = m_archetype_map.find(archetype_id);
		if (it == m_archetype_map.end())
			return false;

		Archetype* archetype = it->second;

		return SortComponents<C>(archetype, std::forward<Comp>(comparison));
	}

	template<IsComponent C, class Comp> requires SameTypeParamDecay<Comp, C, 0, 1>
	inline bool EntityAdmin::SortComponents(EntityID entity_id, Comp&& comparison)
	{
		const auto it = m_entity_archetype_map.find(entity_id);
		if (it == m_entity_archetype_map.end())
			return false;

		Archetype* archetype = it->second.archetype;

		return SortComponents<C>(archetype, std::forward<Comp>(comparison));
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline std::vector<EntityID> EntityAdmin::GetEntitiesWith(bool restricted) const
	{
		constexpr auto component_ids = cu::Sort<ArrComponentIDs<Cs...>>({ GetComponentID<Cs>()... });
		constexpr auto archetype_id = cu::ContainerHash<ArrComponentIDs<Cs...>>()(component_ids);

		return GetEntitiesWith(component_ids, archetype_id, restricted);
	}

	template<class... Cs> requires IsComponents<Cs...>
	inline void EntityAdmin::Reserve(const std::size_t component_count)
	{
		constexpr auto component_ids = cu::Sort<ArrComponentIDs<Cs...>>({ GetComponentID<Cs>()... });
		constexpr auto archetype_id = cu::ContainerHash<ArrComponentIDs<Cs...>>()(component_ids);

		Reserve(component_ids, archetype_id, component_count);
	}

	template<IsComponent C, typename Func>
	inline auto EntityAdmin::RegisterOnAddListener(Func&& func)
	{
		assert(IsComponentRegistered<C>());

		constexpr auto component_id = GetComponentID<C>();
		return	(m_events_add[component_id] += [func = std::forward<Func>(func)](EntityID entity_id, void* ptr)
				{
					func(entity_id, *reinterpret_cast<C*>(ptr));
				});
	}

	template<IsComponent C, typename Func>
	inline auto EntityAdmin::RegisterOnMoveListener(Func&& func)
	{
		assert(IsComponentRegistered<C>());

		constexpr auto component_id = GetComponentID<C>();
		return	(m_events_move[component_id] += [func = std::forward<Func>(func)](EntityID entity_id, void* ptr)
				{ 
					func(entity_id, *reinterpret_cast<C*>(ptr));
				});
	}

	template<IsComponent C, typename Func>
	inline auto EntityAdmin::RegisterOnRemoveListener(Func&& func)
	{
		assert(IsComponentRegistered<C>());

		constexpr auto component_id = GetComponentID<C>();
		return	(m_events_remove[component_id] += [func = std::forward<Func>(func)](EntityID entity_id, void* ptr)
				{
					func(entity_id, *reinterpret_cast<C*>(ptr));
				});
	}

	template<IsComponent C>
	inline void EntityAdmin::DeregisterOnAddListener(const typename EventHandler<>::IDType id)
	{
		assert(IsComponentRegistered<C>());

		constexpr auto component_id = GetComponentID<C>();
		m_events_add[component_id].RemoveID(id);
	}

	template<IsComponent C>
	inline void EntityAdmin::DeregisterOnMoveListener(const typename EventHandler<>::IDType id)
	{
		constexpr auto component_id = GetComponentID<C>();
		m_events_move[component_id].RemoveID(id);
	}

	template<IsComponent C>
	inline void EntityAdmin::DeregisterOnRemoveListener(const typename EventHandler<>::IDType id)
	{
		assert(IsComponentRegistered<C>());

		constexpr auto component_id = GetComponentID<C>();
		m_events_remove[component_id].RemoveID(id);
	}

	template<IsContainer T>
	inline std::vector<EntityID> EntityAdmin::GetEntitiesWith(const T& component_ids, const ArchetypeID archetype_id, bool restricted) const
	{
		assert(cu::IsSorted(component_ids));

		std::vector<EntityID> entities;

		if (!restricted)
		{
			for (const Archetype* archetype : GetArchetypes(component_ids, archetype_id))
			{
				entities.insert(entities.end(),
					archetype->entities.begin(),
					archetype->entities.end());
			}
		}
		else
		{
			const auto it = m_archetype_map.find(archetype_id);
			if (it == m_archetype_map.end())
				return entities;

			entities.insert(entities.end(), 
				it->second->entities.begin(), 
				it->second->entities.end());
		}

		return entities;
	}

	template<IsContainer T>
	inline void EntityAdmin::Reserve(const T& component_ids, const ArchetypeID archetype_id, const std::size_t component_count)
	{
		assert(cu::IsSorted(component_ids));

		Archetype* archetype = GetArchetype(component_ids, archetype_id);
		for (std::size_t i = 0; i < archetype->type.size(); ++i)
		{
			const auto component		= m_component_map[archetype->type[i]].get();
			const auto component_size	= component->GetSize();

			const auto current_size	= archetype->component_data_size[i];
			const auto new_size		= component_count * component_size;

			if (new_size > current_size) // only reserve if larger than current size
			{
				auto new_data = std::make_unique<ByteArray>(new_size);

				for (std::size_t j = 0; j < archetype->entities.size(); ++j)
				{
					component->MoveDestroyData(*this, archetype->entities[j],
						&archetype->component_data[i][j * component_size],
						&new_data[j * component_size]);
				}

				archetype->component_data_size[i]	= new_size;
				archetype->component_data[i]		= std::move(new_data);
			}
		}
	}

	template<IsContainer T>
	inline Archetype* EntityAdmin::GetArchetype(const T& component_ids, const ArchetypeID archetype_id)
	{
		assert(cu::IsSorted(component_ids));

		const auto it = m_archetype_map.find(archetype_id);
		if (it != m_archetype_map.end())
			return it->second;

		return CreateArchetype(component_ids, archetype_id); // archetype does not exist, create new one
	}

	template<IsContainer T>
	inline Archetype* EntityAdmin::CreateArchetype(const T& component_ids, const ArchetypeID archetype_id)
	{
		assert(cu::IsSorted(component_ids));

		auto new_archetype = std::make_unique<Archetype>();

		new_archetype->id	= archetype_id;
		new_archetype->type = { component_ids.begin(), component_ids.end() };

		m_archetype_map[archetype_id] = new_archetype.get();

		for (uint64 i = 0; i < new_archetype->type.size(); ++i) // add empty array for each component in type
		{
			constexpr uint64 DEFAULT_BYTE_SIZE = 128; // default size in bytes to reduce number of reallocations

			new_archetype->component_data.push_back(std::make_unique<ByteArray>(DEFAULT_BYTE_SIZE));
			new_archetype->component_data_size.push_back(DEFAULT_BYTE_SIZE);

			m_component_archetypes_map[new_archetype->type[i]][archetype_id].column = ColumnType(i);
		}

#if defined(VELOX_DEBUG)
		for (const auto& archetype : m_archetypes)
		{
			for (const auto& archetype1 : m_archetypes)
			{
				if (archetype.get() == archetype1.get())
					continue;

				assert(archetype->type != archetype1->type); // no duplicates
			}
		}
#endif

		m_archetype_cache.clear(); // unfortunately for now, we'll have to clear the cache whenever an archetype has been added

		return m_archetypes.emplace_back(std::move(new_archetype)).get();
	}

	template<IsContainer T>
	inline std::span<const Archetype* const> EntityAdmin::GetArchetypes(const T& component_ids, const ArchetypeID archetype_id) const
	{
		const auto it = m_archetype_cache.find(archetype_id);
		if (it != m_archetype_cache.end())
			return it->second;

		std::vector<Archetype*> result;

		for (const ArchetypePtr& archetype : m_archetypes)
		{
			if (std::ranges::includes(archetype->type.begin(), archetype->type.end(), component_ids.begin(), component_ids.end()))
				result.push_back(archetype.get());
		}

		return m_archetype_cache.emplace(archetype_id, result).first->second;
	}

	template<IsComponent C, class Comp>
	inline bool EntityAdmin::SortComponents(Archetype* archetype, Comp&& comparison)
	{
		if (archetype == nullptr)
			return false;

		const auto cit = m_component_archetypes_map.find(GetComponentID<C>());
		if (cit == m_component_archetypes_map.end())
			return false;

		const auto ait = cit->second.find(archetype->id);
		if (ait == cit->second.end())
			return false;

		const ArchetypeRecord& a_record = ait->second;

		C* components = reinterpret_cast<C*>(&archetype->component_data[a_record.column][0]);

		std::vector<std::size_t> indices(archetype->entities.size());
		std::iota(indices.begin(), indices.end(), 0);

		std::ranges::stable_sort(indices.begin(), indices.end(),
			[&comparison, &components](const std::size_t lhs, std::size_t rhs)
			{
				return std::forward<Comp>(comparison)(components[lhs], components[rhs]);
			});

		for (std::size_t i = 0; i < archetype->type.size(); ++i) // sort the components, all need to be sorted
		{
			const auto component_id		= archetype->type[i];
			const auto component		= m_component_map[component_id].get();
			const auto component_size	= component->GetSize();

			ComponentData new_data = std::make_unique<ByteArray>(archetype->component_data_size[i]);

			for (std::size_t j = 0; j < archetype->entities.size(); ++j)
			{
				component->MoveDestroyData(*this, archetype->entities[j],
					&archetype->component_data[i][indices[j] * component_size],
					&new_data[j * component_size]);
			}

			archetype->component_data[i] = std::move(new_data);
		}

		decltype(archetype->entities) new_entities;
		for (std::size_t i = 0; i < archetype->entities.size(); ++i) // now swap the entities
		{
			const std::size_t index = indices[i];
			EntityID entity_id = archetype->entities[index];

			auto it = m_entity_archetype_map.find(entity_id);
			assert(it != m_entity_archetype_map.end()); // should never happen

			it->second.index = IDType(i);
			new_entities.push_back(entity_id);
		}

		archetype->entities = new_entities;

		return true;
	}

	template<IsComponent C>
	inline void EntityAdmin::EraseComponentRef(EntityID entity_id) const
	{
		EraseComponentRef(entity_id, GetComponentID<C>());
	}

	template<IsComponent C>
	inline void EntityAdmin::UpdateComponentRef(EntityID entity_id, C* new_component) const
	{
		UpdateComponentRef(entity_id, GetComponentID<C>(), static_cast<void*>(new_component)); // TODO: CHECK OUT
	}
}