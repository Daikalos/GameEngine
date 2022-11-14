#pragma once

#include <typeinfo>
#include <atomic>

#include "Concepts.h"

namespace vlx::id
{
	template<class T, Arithmetic ReturnType = std::size_t>
	inline static ReturnType GetTypeID() noexcept
	{
		static const ReturnType id = ReturnType(typeid(T).hash_code());
		return id;
	}
}