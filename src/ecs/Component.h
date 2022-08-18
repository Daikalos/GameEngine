#pragma once

#include <SFML/Graphics.hpp>

#include "Identifiers.h"
#include "TypeIdGenerator.h"

namespace fge
{
	class ComponentBase
	{
	public:
		virtual ~ComponentBase() { }

		virtual void ConstructData(unsigned char* data) const = 0;
		virtual void DestroyData(unsigned char* data) const = 0;
		virtual void MoveData(unsigned char* source, unsigned char* destination) const = 0;

		virtual std::size_t GetSize() const = 0;
	};

	template<class C>
	class Component : public ComponentBase
	{
	public:

		virtual void ConstructData(unsigned char* data) const override;
		virtual void DestroyData(unsigned char* data) const override;
		virtual void MoveData(unsigned char* source, unsigned char* destination) const override;

		virtual std::size_t GetSize() const override;

		static ComponentTypeID GetTypeId();
	};
}

