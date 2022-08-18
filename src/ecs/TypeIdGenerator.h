#pragma once

#include "Identifiers.h"

namespace fge
{
	template<class C>
	class TypeIdGenerator // maybe look at re-using IDs
	{
	private:
		static IDType _count; // unique for every instance of C

	public:
		template<class U>
		static const IDType GetNewId()
		{
			static const IDType id_counter = _count++; // unique for every instance of U
			return id_counter;
		}
	};

	template<class C>
	IDType TypeIdGenerator<C>::_count = 0;
}