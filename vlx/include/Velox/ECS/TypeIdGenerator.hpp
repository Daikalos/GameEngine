#pragma once

#include <mutex>
#include <typeinfo>

#include "Identifiers.hpp"

namespace vlx
{
	class TypeIDGenerator
	{
	public:
		template<class U>
		inline static IDType GetUniqueID()
		{
			return static_cast<IDType>(typeid(U).hash_code());
		}
	};
}