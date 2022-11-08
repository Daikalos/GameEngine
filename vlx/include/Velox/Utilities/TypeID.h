#pragma once

#include <typeinfo>

#include "Concepts.h"

namespace vlx::type
{
	template<class T, Arithmetic ReturnType = std::size_t>
	inline static ReturnType GetUniqueID() noexcept
	{
		static const ReturnType id = ReturnType(typeid(T).hash_code());
		return id;
	}
}