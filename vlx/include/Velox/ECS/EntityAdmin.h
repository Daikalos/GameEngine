#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <queue>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"
#include "Archetype.hpp"
#include "System.hpp"

namespace vlx
{
	// forward declarations

	class Entity;

	struct IComponentAlloc;

	template<IsComponent>
	struct ComponentAlloc;

	class IComponentProxy;

	template<IsComponent>
	class ComponentProxy;

	template<class>
	class BaseProxy;

	template<IsComponent...>
	class ComponentSet;

	// global using

	template<class B>
	using BaseProxyPtr		= std::shared_ptr<BaseProxy<B>>;
	template<IsComponent C>
	using ComponentProxyPtr = std::shared_ptr<ComponentProxy<C>>;

	////////////////////////////////////////////////////////////
	// 
	// ECS based on article by Deckhead:
	// https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/ 
	// 
	////////////////////////////////////////////////////////////

	/// <summary>
	///		Data-oriented ECS design. Components are stored in contiguous memory inside of archetypes.
	///		Be warned that adding and removing components from entities is an expensive operation. 
	///		Try to avoid performing such operations on runtime as much as possible and look at using pooling 
	///		instead to prevent removing and adding entities often.
	/// </summary>
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

		using ComponentPtr				= std::unique_ptr<IComponentAlloc>;
		using ArchetypePtr				= std::unique_ptr<Archetype>;

		using SystemsArrayMap			= std::unordered_map<LayerType, std::vector<ISystem*>>;
		using ArchetypesArray			= std::vector<ArchetypePtr>;
		using ArchetypeMap				= std::unordered_map<ArchetypeID, Archetype*>;
		using EntityArchetypeMap		= std::unordered_map<EntityID, Record>;
		using EntityComponentProxyMap	= std::unordered_map<EntityID, std::unordered_map<ComponentTypeID, std::weak_ptr<IComponentProxy>>>;
		using ComponentTypeIDBaseMap	= std::unordered_map<ComponentTypeID, ComponentPtr>;
		using ComponentArchetypesMap	= std::unordered_map<ComponentTypeID, std::unordered_map<ArchetypeID, ArchetypeRecord>>;

		template<IsComponent>
		friend struct ComponentAlloc;

	public:
		VELOX_API EntityAdmin();
		VELOX_API ~EntityAdmin();

	public: 
		/// <summary>
		///		Register the component for later usage. Has to be done before the component can be 
		///		employed in the ECS.
		/// </summary>
		template<IsComponent C>
		void RegisterComponent();

		/// <summary>
		///		Shortcut for registering multiple components.
		/// </summary>
		template<IsComponent... Cs>
		void RegisterComponents();

		/// <summary>
		///		Adds a component to the specified entity. Can also pass optional constructor arguments. 
		///		Returns pointer to the added component if it was successful, otherwise nullptr.
		/// </summary>
		template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
		C* AddComponent(const EntityID entity_id, Args&&... args);

		/// <summary>
		///		Sets the component for the entity directly, component is assumed to exist.
		/// </summary>
		template<IsComponent C>
		C& SetComponent(const EntityID entity_id, const C& new_component);

		/// <summary>
		///		Tries to set the component for entity, will return false if it fails.
		/// </summary>
		template<IsComponent C>
		[[nodiscard]] std::pair<C*, bool> TrySetComponent(const EntityID entity_id, const C& new_component);

		/// <summary>
		///		Removes a component from the specified entity. Will return true if it succeeded in doing such,
		///		otherwise false.
		/// </summary>
		template<IsComponent C>
		bool RemoveComponent(const EntityID entity_id);

		/// <summary>
		///		GetComponent is designed to be as fast as possible without checks to
		///		see if it exists, otherwise, will throw error. Therefore, take some caution when 
		///		using this function. Use e.g. TryGetComponent or GetComponentProxy for better safety.
		/// </summary>
		template<IsComponent C>
		[[nodiscard]] C& GetComponent(const EntityID entity_id) const;

		/// <summary>
		///		Tries to get the component and returns a pair containing the component ptr and success. If it fails, it returns nullptr and false.
		/// </summary>
		template<IsComponent C>
		[[nodiscard]] std::pair<C*, bool> TryGetComponent(const EntityID entity_id) const;

		/// <summary>
		///		Allows for you to retrieve any base class without having to know the type of the child.
		/// 
		/// 	[Incredibly risky, requires base to be first in inheritance, other base classes cannot be 
		///		automatically found without using voodoo magic, for now, offset can be specified to find 
		///		the correct base class in the inheritance order, for example, "class C : B, A", to find A 
		///		you specify offset with the value of sizeof(B)]
		/// </summary>
		template<class B>
		[[nodiscard]] B& GetBase(const EntityID entity_id, const ComponentTypeID child_component_id, const std::size_t offset = 0) const;

		/// <summary>
		/// 	Attempts to get base and returns a pair containing the base ptr and success. If it fails, it returns nullptr and false.
		/// </summary>
		template<class B>
		[[nodiscard]] std::pair<B*, bool> TryGetBase(const EntityID entity_id, const ComponentTypeID child_component_id, const std::size_t offset = 0) const;

		/// <summary>
		///		Returns a proxy for the component whose pointer will remain valid even when the internal data is 
		///		modified. The proxy will internally get the component's new data location once it has been modified.
		/// </summary>
		template<IsComponent C>
		[[nodiscard]] auto GetComponentProxy(const EntityID entity_id) const -> ComponentProxyPtr<C>;

		/// <summary>
		///		Tries to return a component proxy, will most likely always succeed, and will only return false if the 
		///		entity does not exist or other unknown error occurs.
		/// </summary>
		template<IsComponent C>
		[[nodiscard]] auto TryGetComponentProxy(const EntityID entity_id) const -> std::pair<ComponentProxyPtr<C>, bool>;

		/// <summary>
		///		Returns a proxy for the base whose pointer will remain valid even when the internal data is 
		///		modified. The proxy will internally get the base's new data location once it has been modified.
		/// </summary>
		template<class B>
		[[nodiscard]] auto GetBaseProxy(const EntityID entity_id, const ComponentTypeID child_component_id, const std::uint32_t offset = 0) const -> BaseProxyPtr<B>;

		/// <summary>
		///		Tries to return a base proxy, will most likely always succeed, and will only return false if the 
		///		entity does not exist or other unknown error occurs. If the component even exists will be checked in the proxy can be extracted with IsExpired().
		/// </summary>
		template<class B>
		[[nodiscard]] auto TryGetBaseProxy(const EntityID entity_id, const ComponentTypeID child_component_id, const std::uint32_t offset = 0) const -> std::pair<BaseProxyPtr<B>, bool>;

		/// <summary>
		///		Returns true if the entity has the component C, otherwise false.
		/// </summary>
		template<IsComponent C>
		[[nodiscard]] bool HasComponent(const EntityID entity_id) const;

		/// <summary>
		///		Returns true if the component has been registered in the ECS, otherwise false.
		/// </summary>
		template<IsComponent C>
		[[nodiscard]] bool IsComponentRegistered() const;

		/// <summary>
		///		Shortcut for adding multiple components to entity. Cannot pass constructor arguments.
		/// </summary>
		template<IsComponent... Cs>
		void AddComponents(const EntityID entity_id);

		template<IsComponent... Cs>
		void AddComponents(const EntityID entity_id, std::tuple<Cs...>&& tuple);

		template<IsComponent... Cs>
		[[nodiscard]] ComponentSet<Cs...> GetComponents(const EntityID entity_id) const;

		template<IsComponent C>
		[[nodiscard]] ComponentTypeID GetComponentID() const;

	public:
		/// <summary>
		///		Sorts the components for all entities that exactly contains the specified components. 
		///		The components is sorted according to the comparison function. Do note that it will 
		///		also sort all other components the entities may contain in order to maintain order.
		/// </summary>
		template<IsComponents... Cs, class Comp>
		void SortComponents(Comp&& comparison) requires SameTypeParameter<Comp, std::tuple_element_t<0, std::tuple<Cs...>>, 0, 1>;

		/// <summary>
		///		Get all entities that contain the provided components
		/// </summary>
		/// <param name="restricted:">
		///		Get all entities that exactly match the provided components
		/// </param>
		/// <returns></returns>
		template<IsComponents... Cs>
		[[nodiscard]] std::vector<EntityID> GetEntitiesWith(bool restricted = false) const;

		/// <summary>
		///		Increase the capacity in each matching archetype to reduce reallocations. Do note that it
		///		will create new archetypes to reserve if they do not exist.
		/// </summary>
		/// <param name="component_count:">
		///		Number of components to reserve for in the archetypes
		/// </param>
		template<IsComponents... Cs>
		void Reserve(const std::size_t component_count);

	private:
		template<IsComponent C>
		void ResetProxy(const EntityID entity_id) const;

	public:
		VELOX_API [[nodiscard]] EntityID GetNewEntityID();

		VELOX_API auto RegisterEntity(const EntityID entity_id) -> Record&;
		VELOX_API bool IsEntityRegistered(const EntityID entity_id) const;

		VELOX_API void RegisterSystem(const LayerType layer, ISystem* system);

		VELOX_API void RemoveSystem(const LayerType layer, ISystem* system);
		VELOX_API bool RemoveEntity(const EntityID entity_id);

		VELOX_API void RunSystems(const LayerType layer) const;
		VELOX_API void SortSystems(const LayerType layer);

		VELOX_API void AddComponent(const EntityID entity_id, const ComponentTypeID add_component_id);
		VELOX_API bool RemoveComponent(const EntityID entity_id, const ComponentTypeID rmv_component_id);

		VELOX_API bool HasComponent(const EntityID entity_id, const ComponentTypeID component_id) const;

	public:
		/// <summary>
		///		Returns a duplicated entity with the same properties as the specified one
		/// </summary>
		VELOX_API [[nodiscard]] EntityID Duplicate(const EntityID entity_id);

		VELOX_API [[nodiscard]] std::vector<EntityID> GetEntitiesWith(const ComponentIDs& component_ids, bool restricted = false) const;

		VELOX_API void Reserve(const ComponentIDs& component_ids, const std::size_t component_count);

		/// <summary>
		///		Shrinks the ECS by removing all the empty archetypes
		/// </summary>
		/// <param name="extensive:"> 
		///		Perform a complete shrink of the ECS by removing all the extra data space, will most likely 
		///		invalidate all the existing pointers from e.g., GetComponent
		/// </param>
		VELOX_API void Shrink(bool extensive = false);

		VELOX_API void ClearProxies();
		VELOX_API void ClearProxies(const EntityID entity_id);

	private:
		VELOX_API [[nodiscard]] Archetype* GetArchetype(const ComponentIDs& component_ids);
		VELOX_API [[nodiscard]] Archetype* CreateArchetype(const ComponentIDs& component_ids, const ArchetypeID id);

		VELOX_API [[nodiscard]] std::vector<Archetype*> GetArchetypes(const ComponentIDs& component_ids) const;

		VELOX_API void MakeRoom(
			Archetype* archetype,
			const IComponentAlloc* component,
			const std::size_t data_size,
			const std::size_t i);

		VELOX_API void ResetProxy(const EntityID entity_id, const ComponentTypeID component_id) const;

	private:
		EntityID				m_entity_id_counter;	// current id counter for entities
		std::queue<EntityID>	m_reusable_entity_ids;	// reusable ids of entities that have been destroyed

		SystemsArrayMap			m_systems;						// map layer to array of systems (layer allows for controlling the order of calls)
		ArchetypesArray			m_archetypes;					// find matching archetype to update matching entities
		ArchetypeMap			m_archetype_map;				// map set of components to matching archetype that contains such components
		EntityArchetypeMap		m_entity_archetype_map;			// map entity to where its data is located at in the archetype
		ComponentArchetypesMap	m_component_archetypes_map;		// map component to the archetypes it exists in and where all of the components data in the archetype is located at
		ComponentTypeIDBaseMap	m_component_map;				// access to helper functions for modifying each unique component

		mutable EntityComponentProxyMap m_entity_component_proxy_map;
	};
}

#include "ComponentAlloc.hpp"

namespace vlx
{
	template<IsComponent C>
	inline void EntityAdmin::RegisterComponent()
	{
		auto insert = m_component_map.try_emplace(GetComponentID<C>(), std::make_unique<ComponentAlloc<C>>());
		assert(insert.second);
	}

	template<IsComponent... Cs>
	inline void EntityAdmin::RegisterComponents()
	{
		(RegisterComponent<Cs>(), ...);
	}

	template<IsComponent C, typename... Args> requires std::constructible_from<C, Args...>
	inline C* EntityAdmin::AddComponent(const EntityID entity_id, Args&&... args)
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		const auto eit = m_entity_archetype_map.find(entity_id);
		if (eit == m_entity_archetype_map.end())
			return nullptr;

		Record& record = eit->second;
		Archetype* old_archetype = record.archetype;

		C* add_component = nullptr;
		Archetype* new_archetype = nullptr; // we are going to be moving to a new archetype

		const ComponentTypeID add_component_id = GetComponentID<C>();

		if (old_archetype) // already has an attached archetype, define a new archetype
		{
			ComponentIDs new_archetype_id = old_archetype->type; // create copy
			if (!cu::InsertUniqueSorted(new_archetype_id, add_component_id)) // insert while keeping the vector sorted (this should ensure that the archetype is always sorted)
				return nullptr;

			new_archetype = GetArchetype(new_archetype_id);
			assert(new_archetype_id == new_archetype->type);

			const EntityID last_entity_id = old_archetype->entities.back();
			Record& last_record = m_entity_archetype_map[last_entity_id];

			const bool same_entity = (last_entity_id == entity_id);

			for (std::size_t i = 0, j = 0; i < new_archetype_id.size(); ++i) // move all the data from old to new and perform swaps at the same time
			{
				const ComponentTypeID component_id	= new_archetype_id[i];
				const IComponentAlloc* component	= m_component_map[component_id].get();
				const std::size_t& component_size	= component->GetSize();

				const std::size_t current_size		= new_archetype->entities.size() * component_size;
				const std::size_t new_size			= current_size + component_size;

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

					if (!same_entity)
					{
						component->MoveDestroyData(*this, last_entity_id,
							&old_archetype->component_data[j][last_record.index * component_size],
							&old_archetype->component_data[j][record.index * component_size]); // move data to last
					}

					++j;
				}
			}

			assert(add_component != nullptr); // a new component should have been added

			if (!same_entity) // move back to current
			{
				old_archetype->entities[record.index] = old_archetype->entities.back();
				last_record.index = record.index;
			}

			old_archetype->entities.pop_back(); // by only removing the last entity, it means that when the next component is added, it will overwrite the previous
		}
		else // if the entity has no archetype, first component
		{
			ComponentIDs new_archetype_id(1, add_component_id);	// construct archetype with the component id
			new_archetype = GetArchetype(new_archetype_id);		// construct or get archetype using the id

			const IComponentAlloc* component	= m_component_map[add_component_id].get();
			const std::size_t& component_size	= component->GetSize();

			const std::size_t current_size	= new_archetype->entities.size() * component_size;
			const std::size_t new_size		= current_size + component_size;

			if (new_size > new_archetype->component_data_size[0])
				MakeRoom(new_archetype, component, component_size, 0); // make room and move over existing data

			add_component = new(&new_archetype->component_data[0][current_size])
				C(std::forward<Args>(args)...);
		}

		add_component->Created(*this, entity_id);

		new_archetype->entities.push_back(entity_id);
		record.index = static_cast<IDType>(new_archetype->entities.size() - 1);
		record.archetype = new_archetype;

		return add_component;
	}

	template<IsComponent C>
	inline C& EntityAdmin::SetComponent(const EntityID entity_id, const C& new_component)
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		const Record& record = m_entity_archetype_map.find(entity_id)->second;
		const Archetype* archetype = record.archetype;

		const ComponentTypeID& component_id = GetComponentID<C>();

		const auto cit = m_component_archetypes_map.find(component_id);
		const auto ait = cit->second.find(archetype->id);

		C* components = reinterpret_cast<C*>(&archetype->component_data[ait->second.column][0]);
		C& component = components[record.index];

		if (&component == &new_component)
			throw std::runtime_error("cannot set the same component");

		static_cast<IComponent&>(component).Modified(
			*this, entity_id, static_cast<const IComponent&>(new_component));

		component = new_component;

		return component;
	}

	template<IsComponent C>
	inline std::pair<C*, bool> EntityAdmin::TrySetComponent(const EntityID entity_id, const C& new_component)
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		const auto eit = m_entity_archetype_map.find(entity_id);
		if (eit == m_entity_archetype_map.end())
			return { nullptr, false };

		const Record& record = eit->second;
		const Archetype* archetype = record.archetype;

		if (archetype == nullptr)
			return { nullptr, false };

		const ComponentTypeID& component_id = GetComponentID<C>();

		const auto cit = m_component_archetypes_map.find(component_id);
		if (cit == m_component_archetypes_map.end())
			return { nullptr, false };

		const auto ait = cit->second.find(archetype->id);
		if (ait == cit->second.end())
			return { nullptr, false };

		const ArchetypeRecord& a_record = ait->second;

		C* components = reinterpret_cast<C*>(&archetype->component_data[ait->second.column][0]);
		C& component = components[record.index];

		if (&component == &new_component)
			throw std::runtime_error("cant set the same component");

		static_cast<IComponent&>(component).Modified(
			*this, entity_id, static_cast<const IComponent&>(new_component));

		component = new_component;

		return { &component, true };
	}

	template<IsComponent C>
	inline bool EntityAdmin::RemoveComponent(const EntityID entity_id)
	{
		return RemoveComponent(entity_id, GetComponentID<C>());
	}

	template<IsComponent C>
	inline C& EntityAdmin::GetComponent(const EntityID entity_id) const
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		const Record& record = m_entity_archetype_map.find(entity_id)->second;
		const Archetype* archetype = record.archetype;

		const ComponentTypeID& component_id = GetComponentID<C>();

		const auto cit = m_component_archetypes_map.find(component_id);
		const auto ait = cit->second.find(archetype->id);

		C* components = reinterpret_cast<C*>(&archetype->component_data[ait->second.column][0]);

		return components[record.index];
	}

	template<IsComponent C>
	inline std::pair<C*, bool> EntityAdmin::TryGetComponent(const EntityID entity_id) const
	{
		assert(IsComponentRegistered<C>()); // component should be registered

		const auto eit = m_entity_archetype_map.find(entity_id);
		if (eit == m_entity_archetype_map.end())
			return { nullptr, false };

		const Record& record = eit->second;
		const Archetype* archetype = record.archetype;

		if (archetype == nullptr)
			return { nullptr, false };

		const ComponentTypeID& component_id = GetComponentID<C>();

		const auto cit = m_component_archetypes_map.find(component_id);
		if (cit == m_component_archetypes_map.end())
			return { nullptr, false };

		const auto ait = cit->second.find(archetype->id);
		if (ait == cit->second.end())
			return { nullptr, false };

		const ArchetypeRecord& a_record = ait->second;
		C* components = reinterpret_cast<C*>(&archetype->component_data[a_record.column][0]);

		return { &components[record.index], true };
	}

	template<class B>
	inline B& EntityAdmin::GetBase(const EntityID entity_id, const ComponentTypeID child_component_id, const std::size_t offset) const
	{
		const Record& record = m_entity_archetype_map.find(entity_id)->second;
		const Archetype* archetype = record.archetype;

		const auto cit = m_component_archetypes_map.find(child_component_id);
		const auto ait = cit->second.find(archetype->id);

		const IComponentAlloc* component	= m_component_map.find(child_component_id)->second.get();
		const std::size_t& component_size	= component->GetSize();

		auto ptr = &archetype->component_data[ait->second.column][record.index * component_size];
		B* base_component = reinterpret_cast<B*>(ptr + offset);	

		return *base_component;
	}

	template<class B>
	inline std::pair<B*, bool> EntityAdmin::TryGetBase(const EntityID entity_id, const ComponentTypeID child_component_id, const std::size_t offset) const
	{
		const auto eit = m_entity_archetype_map.find(entity_id);
		if (eit == m_entity_archetype_map.end())
			return { nullptr, false };

		const Record& record = eit->second;
		const Archetype* archetype = record.archetype;

		if (archetype == nullptr)
			return { nullptr, false };

		const auto cit = m_component_archetypes_map.find(child_component_id);
		if (cit == m_component_archetypes_map.end())
			return { nullptr, false };

		const auto ait = cit->second.find(archetype->id);
		if (ait == cit->second.end())
			return { nullptr, false };

		const auto iit = m_component_map.find(child_component_id);
		if (iit == m_component_map.end())
			return { nullptr, false };

		const std::size_t& component_size = iit->second->GetSize();

		auto ptr = &archetype->component_data[ait->second.column][record.index * component_size];
		B* base_component = reinterpret_cast<B*>(ptr + offset);

		return { base_component, true };
	}

	template<IsComponent C>
	inline auto EntityAdmin::GetComponentProxy(const EntityID entity_id) const -> ComponentProxyPtr<C>
	{
		assert(IsComponentRegistered<C>());

		auto& component_proxies = m_entity_component_proxy_map[entity_id]; // will construct new if it does not exist
		const ComponentTypeID& component_id = GetComponentID<C>();

		const auto cit = component_proxies.find(component_id);
		if (cit == component_proxies.end() || cit->second.expired()) // it does not yet exist, create new one
		{
			ComponentProxyPtr<C> proxy = std::make_shared<ComponentProxy<C>>(*this, entity_id);
			component_proxies[component_id] = proxy;

			return proxy;
		}

		return std::static_pointer_cast<ComponentProxy<C>>(cit->second.lock());
	}

	template<IsComponent C>
	inline auto EntityAdmin::TryGetComponentProxy(const EntityID entity_id) const -> std::pair<ComponentProxyPtr<C>, bool>
	{
		if (!IsEntityRegistered(entity_id) || !HasComponent<C>(entity_id))
			return { nullptr, false };

		return { GetComponentProxy<C>(entity_id), true};
	}

	template<class B>
	inline auto EntityAdmin::GetBaseProxy(const EntityID entity_id, const ComponentTypeID child_component_id, const std::uint32_t offset) const -> BaseProxyPtr<B>
	{
		auto& component_proxies = m_entity_component_proxy_map[entity_id]; // will construct new if it does not exist

		const auto cit = component_proxies.find(child_component_id);
		if (cit == component_proxies.end() || cit->second.expired()) // it does not yet exist, create new one
		{
			BaseProxyPtr<B> proxy = std::make_shared<BaseProxy<B>>(*this, entity_id, child_component_id, offset);
			component_proxies[child_component_id] = proxy;

			return proxy;
		}

		return std::static_pointer_cast<BaseProxy<B>>(cit->second.lock());
	}

	template<class B>
	inline auto EntityAdmin::TryGetBaseProxy(const EntityID entity_id, const ComponentTypeID child_component_id, const std::uint32_t offset) const -> std::pair<BaseProxyPtr<B>, bool>
	{
		if (!IsEntityRegistered(entity_id) || !HasComponent(entity_id, child_component_id)) // check if entity exists
			return { nullptr, false };

		return { GetBaseProxy<B>(entity_id, child_component_id, offset), true};
	}

	template<IsComponent C>
	inline bool EntityAdmin::HasComponent(const EntityID entity_id) const
	{
		assert(IsComponentRegistered<C>()); // component should be registered
		return HasComponent(entity_id, GetComponentID<C>());
	}

	template<IsComponent C>
	inline bool EntityAdmin::IsComponentRegistered() const
	{
		return m_component_map.contains(GetComponentID<C>());
	}

	template<IsComponent... Cs>
	inline void EntityAdmin::AddComponents(const EntityID entity_id)
	{
		(AddComponent(entity_id, GetComponentID<Cs>()), ...);
	}

	template<IsComponent... Cs>
	inline void EntityAdmin::AddComponents(const EntityID entity_id, std::tuple<Cs...>&& tuple)
	{
		AddComponents<Cs...>(entity_id);
	}


	template<IsComponent... Cs>
	inline ComponentSet<Cs...> EntityAdmin::GetComponents(const EntityID entity_id) const
	{
		return ComponentSet<Cs...>(GetComponentProxy<Cs>(entity_id)...);
	}

	template<IsComponent C>
	inline ComponentTypeID EntityAdmin::GetComponentID() const
	{
		return ComponentAlloc<C>::GetTypeID();
	}

	template<IsComponents... Cs, class Comp>
	inline void EntityAdmin::SortComponents(Comp&& comparison) requires SameTypeParameter<Comp, std::tuple_element_t<0, std::tuple<Cs...>>, 0, 1>
	{
		const ComponentIDs component_ids = cu::Sort<ComponentTypeID>({ { GetComponentID<Cs>()... } });
		const ArchetypeID archetype_id = cu::VectorHash<ComponentIDs>()(component_ids);

		const auto it = m_archetype_map.find(archetype_id);
		if (it == m_archetype_map.end())
			return;

		Archetype* archetype = it->second.front();

		if (archetype->id != archetype_id)
			throw std::runtime_error("the specified archetype does not exist");

		using C = std::tuple_element_t<0, std::tuple<Cs...>>; // the component that is meant to be sorted
		const ComponentTypeID& component_id = GetComponentID<C>();

		const auto cit = m_component_archetypes_map.find(component_id);
		if (cit == m_component_archetypes_map.end())
			return;

		const auto ait = cit->second.find(archetype->id);
		if (ait == cit->second.end())
			return;

		const ArchetypeRecord& a_record = ait->second;

		C* components = reinterpret_cast<C*>(&archetype->component_data[a_record.column][0]);

		std::vector<std::size_t> indices(archetype->entities.size());
		std::iota(indices.begin(), indices.end(), 0);

		std::stable_sort(indices.begin(), indices.end(),
			[&comparison, &components](const std::size_t lhs, std::size_t rhs)
			{
				return std::forward<Comp>(comparison)(components[lhs], components[rhs]);
			});

		for (std::size_t i = 0; i < archetype->type.size(); ++i) // sort the components, all need to be sorted
		{
			const ComponentTypeID component_id	= archetype->type[i];
			const IComponentAlloc* component	= m_component_map[component_id].get();
			constexpr std::size_t component_size	= component->GetSize();

			ComponentData new_data = std::make_unique<ByteArray>(archetype->component_data_size[i]);

			for (std::size_t j = 0; j < archetype->entities.size(); ++j)
			{
				component->MoveDestroyData(*this, archetype->entities[j],
					&archetype->component_data[i][indices[j] * component_size],
					&new_data[j * component_size]);
			}

			archetype->component_data[i] = std::move(new_data);
		}

		std::vector<EntityID> new_entities;
		new_entities.reserve(archetype->entities.size());

		for (std::size_t i = 0; i < archetype->entities.size(); ++i) // now swap the entities
		{
			const std::size_t index = indices[i];
			const EntityID entity_id = archetype->entities[index];

			auto it = m_entity_archetype_map.find(entity_id);
			assert(it != m_entity_archetype_map.end()); // should never happen

			it->second.index = i;
			new_entities.push_back(entity_id);
		}

		archetype->entities = new_entities;
	}

	template<IsComponents... Cs>
	inline void EntityAdmin::Reserve(const std::size_t component_count)
	{
		Reserve(cu::Sort<ComponentTypeID>({ { GetComponentID<Cs>()... } }), component_count);
	}

	template<IsComponents... Cs>
	inline std::vector<EntityID> EntityAdmin::GetEntitiesWith(bool restricted) const
	{
		return GetEntitiesWith(cu::Sort<ComponentTypeID>({ { GetComponentID<Cs>()... } }), restricted);
	}

	template<IsComponent C>
	inline void EntityAdmin::ResetProxy(const EntityID entity_id) const
	{
		ResetProxy(entity_id, GetComponentID<C>());
	}
}






// -- old add component --

//// cant add multiple of the same component to same entity
//if (std::find(old_archetype->type.begin(), old_archetype->type.end(), new_comp_type_id) != old_archetype->type.end())
//	return nullptr;

//ArchetypeID new_archetype_id = old_archetype->type;	  // create copy
//cu::InsertSorted(new_archetype_id, new_comp_type_id); // insert while keeping the vector sorted (this should ensure that the archetype is always sorted)

//new_archetype = GetArchetype(new_archetype_id); // create the new archetype

//for (std::size_t i = 0; i < new_archetype_id.size(); ++i) // for every component in the new archetype
//{
//	const ComponentTypeID& component_id = new_archetype_id[i];
//	const ComponentBase* component = m_component_map[component_id].get();
//	const std::size_t& component_size = component->GetSize();

//	const std::size_t current_size = new_archetype->entities.size() * component_size;
//	const std::size_t new_size = current_size + component_size;

//	if (new_size > new_archetype->component_data_size[i])
//		MakeRoom(new_archetype, component, component_size, i); // make room to fit old data

//	bool found = MoveComponent(old_archetype, new_archetype, component, component_id, record.index * component_size, current_size, i);

//	if (!found) // if there was no old instance we could copy our data to, construct a new component
//	{
//		new_component = new (&new_archetype->component_data[i][current_size])
//			C(std::forward<Args>(args)...);
//	}
//}

//const ArchetypeID& old_archetype_id = old_archetype->type;
//for (std::size_t i = 0; i < old_archetype_id.size(); ++i) // we'll have to remove this entity's data in the old as it has been assigned a new one
//{
//	const ComponentTypeID& component_id = old_archetype_id[i];
//	const ComponentBase* component = m_component_map[component_id].get();

//	EraseComponent(old_archetype, component, record.index, i);
//}

//auto it = std::find(old_archetype->entities.begin(), old_archetype->entities.end(), entity_id); // look at swap and pop

//std::for_each(it, old_archetype->entities.end(),
//	[this, &old_archetype](const EntityID& eid)
//	{
//		--m_entity_archetype_map[eid].index;
//	});

//old_archetype->entities.erase(it); // may result in dangling empty archetypes, but is better than having to continously reallocate new memory



// -- old remove component --

//ArchetypeID new_archetype_id = old_archetype->type; // create copy

//new_archetype_id.erase(std::remove(
//	new_archetype_id.begin(), new_archetype_id.end(), comp_type_id), new_archetype_id.end()); // remove this component

////std::sort(new_archetype_id.begin(), new_archetype_id.end()); should not be needed

//Archetype* new_archetype = GetArchetype(new_archetype_id);

//for (std::size_t i = 0; i < new_archetype_id.size(); ++i)
//{
//	const ComponentTypeID& component_id = new_archetype_id[i];
//	const ComponentBase* component = m_component_map[component_id].get();
//	const std::size_t& component_size = component->GetSize();

//	const std::size_t current_size = new_archetype->entities.size() * component_size;
//	const std::size_t new_size = current_size + component_size;

//	if (new_size > new_archetype->component_data_size[i])
//		MakeRoom(new_archetype, component, component_size, i); // make room to fit data

//	component->ConstructData(&new_archetype->component_data[i][current_size]);

//	bool found = MoveComponent(old_archetype, new_archetype, component, component_id, record.index * component_size, current_size, i);

//	assert(found);
//}

//const ArchetypeID& old_archetype_id = old_archetype->type;
//for (std::size_t i = 0; i < old_archetype_id.size(); ++i)
//{
//	const ComponentTypeID& component_id = old_archetype_id[i];
//	const ComponentBase* component = m_component_map[component_id].get();

//	if (component_id == comp_type_id)
//	{
//		const std::size_t& component_size = component->GetSize();
//		component->DestroyData(&old_archetype->component_data[i][record.index * component_size]);
//	}

//	EraseComponent(old_archetype, component, record.index, i);
//}

//auto it = std::find(
//	old_archetype->entities.begin(),
//	old_archetype->entities.end(), entity_id);

//std::for_each(it, old_archetype->entities.end(),
//	[this](const EntityID& eid)
//	{
//		--m_entity_archetype_map[eid].index;
//	});

//old_archetype->entities.erase(std::remove(
//	old_archetype->entities.begin(), old_archetype->entities.end(), entity_id),
//	old_archetype->entities.end());

//new_archetype->entities.push_back(entity_id);
//record.index = new_archetype->entities.size() - 1;
//record.archetype = new_archetype;