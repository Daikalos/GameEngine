#pragma once

#include <SFML/Graphics.hpp>

#include "Identifiers.hpp"
#include "TypeIdGenerator.hpp"

namespace vlx
{
	////////////////////////////////////////////////////////////
	// unsigned char* is used for representing a binary data 
	// buffer, where each element is a single byte. 
	////////////////////////////////////////////////////////////
	class ComponentBase
	{
	public:
		virtual ~ComponentBase() { }

		virtual void ConstructData(unsigned char* data) const = 0;
		virtual void DestroyData(unsigned char* data) const = 0;
		virtual void MoveData(unsigned char* source, unsigned char* destination) const = 0;

		virtual void MoveDestroyData(unsigned char* source, unsigned char* destination) const = 0;

		virtual constexpr std::size_t GetSize() const noexcept = 0;
	};

	template<class C>
	class Component : public ComponentBase
	{
	public:
		virtual void ConstructData(unsigned char* data) const override;
		virtual void DestroyData(unsigned char* data) const override;
		virtual void MoveData(unsigned char* source, unsigned char* destination) const override;

		virtual void MoveDestroyData(unsigned char* source, unsigned char* destination) const override;

		virtual constexpr std::size_t GetSize() const noexcept override;

		static ComponentTypeID GetTypeId();
	};

	template<class C>
	inline void Component<C>::ConstructData(unsigned char* data) const
	{
		new (&data[0]) C(); // construct object C at data in memory which will give its values
	}

	template<class C>
	inline void Component<C>::DestroyData(unsigned char* data) const
	{
		C* data_location = std::launder(reinterpret_cast<C*>(data)); // launder allows for changing the type of object (makes the type cast legal)
		data_location->~C();
	}

	template<class C>
	inline void Component<C>::MoveData(unsigned char* source, unsigned char* destination) const
	{
		new (&destination[0]) C(std::move(*reinterpret_cast<C*>(source))); // move the data in src by constructing a object at dest with the values from src
	}

	template<class C>
	inline void Component<C>::MoveDestroyData(unsigned char* source, unsigned char* destination) const
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

