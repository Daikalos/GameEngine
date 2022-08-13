#pragma once

#include "Identifiers.h"

namespace fge
{
	template<class C>
	class TypeIdGenerator // maybe look at re-using IDs
	{
	private:
		static IDType _count; // unique for every instance of the template

	public:
		template<class U>
		static const IDType get_new_id()
		{
			static const IDType id_counter = _count++;
			return id_counter;
		}
	};

	template<class C>
	IDType TypeIdGenerator<C>::_count = 0;
}