#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Utilities.hpp>

#include "Identifiers.hpp"
#include "TypeIdGenerator.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// 
	// ComponentAlloc is a helper class for constructing and moving
	// data depending on a specific type
	// 
	////////////////////////////////////////////////////////////
	struct IComponentAlloc
	{
		virtual void ConstructData(DataPtr data) const = 0;
		virtual void DestroyData(DataPtr data) const = 0;
		virtual void MoveData(DataPtr source, DataPtr destination) const = 0;
		virtual void SwapData(DataPtr d0, DataPtr d1) const = 0;
		virtual void MoveDestroyData(DataPtr source, DataPtr destination) const = 0;
		virtual constexpr std::size_t GetSize() const noexcept = 0;
	};

	template<IsComponent C>
	struct ComponentAlloc : public IComponentAlloc
	{
		void ConstructData(DataPtr data) const override;
		void DestroyData(DataPtr data) const override;
		void MoveData(DataPtr source, DataPtr destination) const override;
		void SwapData(DataPtr d0, DataPtr d1) const override;
		void MoveDestroyData(DataPtr source, DataPtr destination) const override;
		[[nodiscard]] constexpr std::size_t GetSize() const noexcept override;

		[[nodiscard]] static ComponentTypeID GetTypeID();
	};

	template<IsComponent C>
	void ComponentAlloc<C>::ConstructData(DataPtr data) const
	{
		new (data) C();
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::DestroyData(DataPtr data) const
	{
		C* data_location = std::launder(reinterpret_cast<C*>(data)); // launder allows for changing the type of object (makes the type cast legal in certain cases)
		data_location->~C();
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::MoveData(DataPtr source, DataPtr destination) const
	{
		new (destination) C(std::move(*reinterpret_cast<C*>(source))); // move the data in src by constructing a object at dest with the values from src
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::SwapData(DataPtr d0, DataPtr d1) const
	{
		constexpr auto size = sizeof(C);

		std::byte temp[size];
		std::memcpy(temp, d0, size);

		std::memcpy(d0, d1, size); // swaps the contents of two byte arrays
		std::memcpy(d1, temp, size);
	}

	template<IsComponent C>
	inline void ComponentAlloc<C>::MoveDestroyData(DataPtr source, DataPtr destination) const
	{
		MoveData(source, destination);
		DestroyData(source);
	}

	template<IsComponent C>
	inline constexpr std::size_t ComponentAlloc<C>::GetSize() const noexcept
	{
		return sizeof(C);
	}

	template<IsComponent C>
	inline ComponentTypeID ComponentAlloc<C>::GetTypeID()
	{
		return TypeIdGenerator<IComponentAlloc>::GetNewID<C>();
	}
}

