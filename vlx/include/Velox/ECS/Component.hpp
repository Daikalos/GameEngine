#pragma once

#include <SFML/Graphics.hpp>

#include "Identifiers.hpp"
#include "TypeIdGenerator.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// 
	// ComponentBase is a helper class for constructing and moving
	// data depending on a specific type
	// 
	////////////////////////////////////////////////////////////
	struct IComponent
	{
		virtual void ConstructData(DataPtr data) const = 0;
		virtual void DestroyData(DataPtr data) const = 0;
		virtual void MoveData(DataPtr source, DataPtr destination) const = 0;
		virtual void SwapData(DataPtr d0, DataPtr d1) const = 0;
		virtual void MoveDestroyData(DataPtr source, DataPtr destination) const = 0;
		virtual constexpr std::size_t GetSize() const noexcept = 0;
	};

	template<class C>
	struct Component : public IComponent
	{
		void ConstructData(DataPtr data) const override;
		void DestroyData(DataPtr data) const override;
		void MoveData(DataPtr source, DataPtr destination) const override;
		void SwapData(DataPtr d0, DataPtr d1) const override;
		void MoveDestroyData(DataPtr source, DataPtr destination) const override;
		[[nodiscard]] constexpr std::size_t GetSize() const noexcept override;

		[[nodiscard]] static ComponentTypeID GetTypeID();
	};

	template<class C>
	void Component<C>::ConstructData(DataPtr data) const
	{
		new (data) C();
	}

	template<class C>
	inline void Component<C>::DestroyData(DataPtr data) const
	{
		C* data_location = std::launder(reinterpret_cast<C*>(data)); // launder allows for changing the type of object (makes the type cast legal in certain cases)
		data_location->~C();
	}

	template<class C>
	inline void Component<C>::MoveData(DataPtr source, DataPtr destination) const
	{
		new (destination) C(std::move(*reinterpret_cast<C*>(source))); // move the data in src by constructing a object at dest with the values from src
	}

	template<class C>
	inline void Component<C>::SwapData(DataPtr d0, DataPtr d1) const
	{
		constexpr auto size = sizeof(C);

		std::byte temp[size];
		std::memcpy(temp, d0, size);

		std::memcpy(d0, d1, size); // swaps the contents of two byte arrays
		std::memcpy(d1, temp, size);
	}

	template<class C>
	inline void Component<C>::MoveDestroyData(DataPtr source, DataPtr destination) const
	{
		MoveData(source, destination);
		DestroyData(source);
	}

	template<class C>
	inline constexpr std::size_t Component<C>::GetSize() const noexcept
	{
		return sizeof(C);
	}

	template<class C>
	inline ComponentTypeID Component<C>::GetTypeID()
	{
		return TypeIdGenerator<IComponent>::GetNewID<C>();
	}
}

