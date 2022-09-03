#pragma once

#include <mutex>

#include "Identifiers.hpp"

namespace vlx
{
	template<class C>
	class TypeIdGenerator // maybe look at re-using IDs
	{
	public:
		template<class U>
		static const IDType GetNewId()
		{
			std::lock_guard lock(m_mutex);
			
			static const IDType id_counter = m_count++; // unique for every instance of U
			return id_counter;
		}

	private:
		static IDType		m_count; // unique for every instance of C
		static std::mutex	m_mutex;
	};

	template<class C>
	IDType TypeIdGenerator<C>::m_count = 1;

	template<class C>
	std::mutex TypeIdGenerator<C>::m_mutex;
}