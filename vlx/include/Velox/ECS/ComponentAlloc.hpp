#pragma once

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include "ComponentEvents.h"
#include "IComponentAlloc.hpp"
#include "EntityAdmin.h"

namespace vlx
{
	template<IsComponent C>
	struct ComponentAlloc final : public IComponentAlloc
	{
		void ConstructData(		const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const override;
		void DestroyData(		const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const override;
		void MoveData(			const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const override;
		void MoveDestroyData(	const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const override;
		void CopyData(			const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const override;
		void SwapData(			const EntityAdmin& entity_admin, EntityID entity_id, DataPtr d0, DataPtr d1) const override;
		void Shutdown(			const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const override;

		NODISC constexpr std::size_t GetSize() const noexcept override;

		NODISC static constexpr ComponentTypeID GetTypeID() noexcept;
	};

	template<IsComponent C>
	inline void ComponentAlloc<C>::ConstructData(const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const
	{
		C* data_location = new (data) C();

		if constexpr (HasEvent<C, CreatedEvent>)
			data_location->Created(entity_admin, entity_id);

		constexpr auto component_id = ComponentAlloc<C>::GetTypeID();

		entity_admin.CallOnAddEvent(component_id, entity_id, static_cast<void*>(data_location));
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::DestroyData(const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const
	{
		C* data_location = std::launder(reinterpret_cast<C*>(data)); // launder allows for changing the type of object (makes the type cast legal in certain cases)

		if constexpr (HasEvent<C, DestroyedEvent>)
			data_location->Destroyed(entity_admin, entity_id); // call associated event

		constexpr auto component_id = ComponentAlloc<C>::GetTypeID();

		entity_admin.CallOnRemoveEvent(component_id, entity_id, static_cast<void*>(data_location));
		entity_admin.EraseComponentRef(entity_id, component_id); // make sure that current references are reset

		data_location->~C(); // now destroy
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::MoveData(const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const
	{
		C* data_location = new (destination) C(std::move(*reinterpret_cast<C*>(source))); // move the data in src by constructing a object at dest with the values from src

		if constexpr (HasEvent<C, MovedEvent>)
			data_location->Moved(entity_admin, entity_id); // call associated event

		constexpr auto component_id = ComponentAlloc<C>::GetTypeID();

		entity_admin.CallOnMoveEvent(component_id, entity_id, static_cast<void*>(data_location));
		entity_admin.UpdateComponentRef(entity_id, component_id, static_cast<void*>(data_location)); // update the current references
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::CopyData(const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const
	{
		C* data_location = new (destination) C(*reinterpret_cast<const C*>(source));

		if constexpr (HasEvent<C, CopiedEvent>)
			data_location->Copied(entity_admin, entity_id);
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::SwapData(const EntityAdmin& entity_admin, EntityID entity_id, DataPtr d0, DataPtr d1) const
	{
		// * not really used right now, could be handy in the future

		constexpr auto size = sizeof(C);

		std::byte temp[size];
		std::memcpy(temp, d0, size);

		std::memcpy(d0, d1, size); // swaps the contents of two byte arrays
		std::memcpy(d1, temp, size);
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::Shutdown(const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const
	{
		C* data_location = std::launder(reinterpret_cast<C*>(data)); 

		if constexpr (HasEvent<C, ShutdownEvent>)
			data_location->Shutdown(entity_admin, entity_id); // call associated event

		data_location->~C(); // call destructor to clear possible leaks
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::MoveDestroyData(const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const
	{
		C* source_location	= std::launder(reinterpret_cast<C*>(source));		// Retrieve pointer to source location
		C* dest_location	= new (destination) C(std::move(*source_location));	// move the data in src by constructing a object at dest with the values from src

		if constexpr (HasEvent<C, MovedEvent>)
			dest_location->Moved(entity_admin, entity_id);	// call associated event

		constexpr auto component_id = ComponentAlloc<C>::GetTypeID();

		entity_admin.CallOnMoveEvent(component_id, entity_id, static_cast<void*>(dest_location));
		entity_admin.UpdateComponentRef(entity_id, component_id, static_cast<void*>(dest_location)); // update the current references with the new data

		source_location->~C(); // destroy data
	}

	template<IsComponent C>
	inline constexpr std::size_t ComponentAlloc<C>::GetSize() const noexcept
	{
		return sizeof(C);
	}

	template<IsComponent C>
	inline constexpr ComponentTypeID ComponentAlloc<C>::GetTypeID() noexcept
	{
		return ComponentTypeID(id::Type<C>::ID());
	}
}