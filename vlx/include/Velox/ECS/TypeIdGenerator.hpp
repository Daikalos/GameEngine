#pragma once

#include <mutex>
#include <typeinfo>

#include "Identifiers.hpp"

namespace vlx
{
	class TypeIDGenerator
	{
	public:
		template<class T>
		inline static IDType GetUniqueID() noexcept
		{
			static const IDType id = static_cast<IDType>(typeid(T).hash_code());
			return id;
		}
	};
}