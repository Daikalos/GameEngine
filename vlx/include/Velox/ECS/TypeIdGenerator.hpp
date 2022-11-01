#pragma once

#include <mutex>

#include "Identifiers.hpp"

namespace vlx
{
	class TypeIDGenerator
	{
	public:
		template<class U>
		inline static IDType GetNewID()
		{
			return static_cast<IDType>(typeid(U).hash_code());
		}
	};
}