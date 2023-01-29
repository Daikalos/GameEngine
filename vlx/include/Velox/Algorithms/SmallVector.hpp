#pragma once

#include <array>
#include <vector>

namespace vlx
{
	template<class T, std::size_t N, class Alloc = std::allocator<T>>
	class SmallVector
	{
	public:
		using value_type		= T;
		using reference			= T&;
		using const_reference	= const T&;
		using pointer			= T*;
		using const_pointer		= const T*;
		using allocator_type	= Alloc;
		using size_type			= std::size_t;

	public:
		SmallVector() = default;

		explicit SmallVector(const size_type count, const_reference value = T(), const Alloc& alloc = Allocator());

		SmallVector(const )

	private:
		std::array<T, N>		m_stack;
		std::vector<T, Alloc>	m_heap;
		size_type				m_size;
	};
}