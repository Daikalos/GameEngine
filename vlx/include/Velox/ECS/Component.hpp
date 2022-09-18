#pragma once

#include <SFML/Graphics.hpp>

#include "Identifiers.hpp"
#include "TypeIdGenerator.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// DataPtr is used for representing a binary data 
	// buffer, where each element is a single byte. 
	////////////////////////////////////////////////////////////
	class ComponentBase
	{
	public:
		virtual ~ComponentBase() { }

		virtual void ConstructData(DataPtr data) const = 0;
		virtual void DestroyData(DataPtr data) const = 0;
		virtual void MoveData(DataPtr source, DataPtr destination) const = 0;

		virtual void MoveDestroyData(DataPtr source, DataPtr destination) const = 0;

		virtual constexpr std::size_t GetSize() const noexcept = 0;
	};

	template<class C>
	class Component : public ComponentBase
	{
	public:
		virtual void ConstructData(DataPtr data) const override;
		virtual void DestroyData(DataPtr data) const override;
		virtual void MoveData(DataPtr source, DataPtr destination) const override;

		////////////////////////////////////////////////////////////
		// Small helper function for moving and then destroying 
		// source
		////////////////////////////////////////////////////////////
		virtual void MoveDestroyData(DataPtr source, DataPtr destination) const override;

		virtual [[nodiscard]] constexpr std::size_t GetSize() const noexcept override;

		static [[nodiscard]] ComponentTypeID GetTypeId();
	};

	template<class C>
	inline void Component<C>::ConstructData(DataPtr data) const
	{
		new (data) C(); // construct object C at data in memory which will give its values
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
	inline ComponentTypeID Component<C>::GetTypeId()
	{
		return TypeIdGenerator<ComponentBase>::GetNewId<C>();
	}
}

