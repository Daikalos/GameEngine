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

		virtual void construct_data(unsigned char* data) const = 0;
		virtual void destroy_data(unsigned char* data) const = 0;
		virtual void move_data(unsigned char* source, unsigned char* destination) const = 0;

		virtual std::size_t get_size() const = 0;
	};

	template<class T>
	class Component : public ComponentBase
	{
	public:
		virtual void construct_data(unsigned char* data) const override;
		virtual void destroy_data(unsigned char* data) const override;
		virtual void move_data(unsigned char* source, unsigned char* destination) const override;

		virtual std::size_t get_size() const override;

		static ComponentTypeID get_type_id();
	};
}

