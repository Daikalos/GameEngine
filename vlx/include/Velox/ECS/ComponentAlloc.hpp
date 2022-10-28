#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"
#include "TypeIdGenerator.hpp"
#include "IComponent.h"

namespace vlx
{
	class EntityAdmin;

	/// <summary>
	/// ComponentAlloc is a helper class for altering data according to a specific type
	/// </summary>
	struct IComponentAlloc
	{
		virtual void ConstructData(		const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr data) const = 0;
		virtual void DestroyData(		const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr data) const = 0;
		virtual void MoveData(			const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr source, DataPtr destination) const = 0;
		virtual void MoveDestroyData(	const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr source, DataPtr destination) const = 0;
		virtual void SwapData(			const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr d0, DataPtr d1) const = 0;

		virtual constexpr std::size_t GetSize() const noexcept = 0;
	};

	template<IsComponent C>
	struct ComponentAlloc final : public IComponentAlloc
	{
		void ConstructData(		const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr data) const override;
		void DestroyData(		const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr data) const override;
		void MoveData(			const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr source, DataPtr destination) const override;
		void MoveDestroyData(	const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr source, DataPtr destination) const override;
		void SwapData(			const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr d0, DataPtr d1) const override;

		[[nodiscard]] constexpr std::size_t GetSize() const noexcept override;

		[[nodiscard]] static const ComponentTypeID GetTypeID();
	};
}

#include "EntityAdmin.h" // include here to use declarations

namespace vlx
{
	template<IsComponent C>
	void ComponentAlloc<C>::ConstructData(const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr data) const
	{
		C* data_location = new (data) C();
		static_cast<IComponent*>(data_location)->Created(entity_admin, entity_id);
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::DestroyData(const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr data) const
	{
		C* data_location = std::launder(reinterpret_cast<C*>(data)); // launder allows for changing the type of object (makes the type cast legal in certain cases)
		static_cast<IComponent*>(data_location)->Destroyed(entity_admin, entity_id);
		data_location->~C(); // now destroy

		entity_admin.ResetProxy<C>(entity_id); // need to reset proxy now that the component has been altered
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::MoveData(const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr source, DataPtr destination) const
	{
		C* data_location = new (destination) C(std::move(*reinterpret_cast<C*>(source))); // move the data in src by constructing a object at dest with the values from src
		static_cast<IComponent*>(data_location)->Moved(entity_admin, entity_id);

		entity_admin.ResetProxy<C>(entity_id);
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::SwapData(const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr d0, DataPtr d1) const
	{
		// * not really used right now, could be handy in the future

		constexpr auto size = sizeof(C);

		std::byte temp[size];
		std::memcpy(temp, d0, size);

		std::memcpy(d0, d1, size); // swaps the contents of two byte arrays
		std::memcpy(d1, temp, size);
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::MoveDestroyData(const EntityAdmin& entity_admin, const EntityID entity_id, DataPtr source, DataPtr destination) const
	{
		MoveData(entity_admin, entity_id, source, destination);

		C* source_location = std::launder(reinterpret_cast<C*>(source)); // destroy data without calling the Destroy event
		source_location->~C();
	}

	template<IsComponent C>
	inline constexpr std::size_t ComponentAlloc<C>::GetSize() const noexcept
	{
		return sizeof(C);
	}

	template<IsComponent C>
	inline const ComponentTypeID ComponentAlloc<C>::GetTypeID()
	{
		return TypeIdGenerator<IComponentAlloc>::GetNewID<C>();
	}
}