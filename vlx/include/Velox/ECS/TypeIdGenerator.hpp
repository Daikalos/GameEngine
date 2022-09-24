#pragma once

#include <mutex>

#include "Identifiers.hpp"

namespace vlx
{
	template<class C>
	class TypeIdGenerator
	{
	public:
		template<class U>
		static const IDType GetNewId()
		{
			std::lock_guard lock(m_mutex); // thread-safe way to get unique id
			
			static const IDType id_counter = m_count++; // unique for every instance of U
			return id_counter;
		}

	private:
		static IDType		m_count; // unique for every instance of C
		static std::mutex	m_mutex;
	};

	template<class C>
	IDType TypeIdGenerator<C>::m_count = 1; // 0 is reserved for null

	template<class C>
	std::mutex TypeIdGenerator<C>::m_mutex;
}