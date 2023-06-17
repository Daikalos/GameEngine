#pragma once

#include <array>
#include <vector>
#include <iterator>

#include <Velox/Config.hpp>

namespace vlx
{
	template<class T, int DIR>
	class SmallIterator
	{
	public:
		using iterator_concept	= std::contiguous_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;
		using difference_type	= std::ptrdiff_t;
		using value_type		= std::remove_const_t<T>;
		using element_type		= T;
		using pointer			= T*;
		using reference			= T&;

	public:
		SmallIterator() noexcept = default;
		explicit SmallIterator(pointer ptr) noexcept : m_ptr(ptr) {}

		template<int DIR2> // able to convert non-const to const, but not the other way around
		SmallIterator(const SmallIterator<value_type, DIR2>& rhs) noexcept : m_ptr(rhs.m_ptr) {};

		template<int DIR2>
		SmallIterator& operator=(const SmallIterator<value_type, DIR2>& rhs) noexcept { m_ptr = rhs.m_ptr; };

	public:
		reference operator*() const noexcept	{ return *m_ptr; }
		pointer operator->() const noexcept		{ return m_ptr; }

		SmallIterator& operator++() noexcept	{ m_ptr += DIR; return *this; }
		SmallIterator& operator--() noexcept	{ m_ptr -= DIR; return *this; }

		SmallIterator operator++(int) noexcept	{ SmallIterator temp = *this; ++(*this); return temp; }
		SmallIterator operator--(int) noexcept	{ SmallIterator temp = *this; --(*this); return temp; }

		SmallIterator& operator+=(const difference_type i) noexcept { m_ptr += i; return *this; }
		SmallIterator& operator-=(const difference_type i) noexcept { return *this += -i; }

		NODISC difference_type operator-(const SmallIterator& rhs) const noexcept { return (m_ptr - rhs.m_ptr) * DIR; }

		NODISC SmallIterator operator+(const difference_type i) const noexcept { return (SmallIterator(*this) += i * DIR); }
		NODISC SmallIterator operator-(const difference_type i) const noexcept { return (SmallIterator(*this) += -i * DIR); }

		reference operator[](const difference_type i) const noexcept { return *(*this + i * DIR); }

		template<class U, int DIR2>
		NODISC bool operator==(const SmallIterator<U, DIR2>& rhs) const noexcept { return m_ptr == rhs.m_ptr; }

		template<class U, int DIR2>
		NODISC std::strong_ordering operator<=>(const SmallIterator<U, DIR2>& rhs) const noexcept
		{
			return (DIR == 1) ? (m_ptr <=> rhs.m_ptr) : 0 <=> (m_ptr <=> rhs.m_ptr);
		}

		NODISC friend SmallIterator operator+(const difference_type lhs, const SmallIterator& rhs)
		{
			return (rhs + lhs);
		}

	private:
		pointer m_ptr {nullptr};

		template<class U, int DIR2>
		friend class SmallIterator;
	};

	template<class T, std::size_t N = 32, class Alloc = std::allocator<T>>
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
		using difference_type	= std::ptrdiff_t;

		using iterator					= SmallIterator<value_type, 1>;
		using const_iterator			= SmallIterator<const value_type, 1>;
		using reverse_iterator			= SmallIterator<value_type, -1>;
		using const_reverse_iterator	= SmallIterator<const value_type, -1>;

	public:
		constexpr SmallVector() = default;
		~SmallVector() = default;

		constexpr explicit SmallVector(size_type size, const_reference value = value_type(), const allocator_type& alloc = allocator_type());
		template<class Iter> requires std::contiguous_iterator<Iter>
		constexpr SmallVector(Iter first, Iter last, const allocator_type& alloc = allocator_type());
		constexpr SmallVector(const SmallVector& other, const allocator_type& alloc = allocator_type());
		constexpr SmallVector(SmallVector&& other, const allocator_type& alloc = allocator_type()) noexcept;
		constexpr SmallVector(std::initializer_list<value_type> init_list, const allocator_type& alloc = allocator_type());

		constexpr auto operator=(const SmallVector& rhs) -> SmallVector&;
		constexpr auto operator=(SmallVector&& rhs) noexcept -> SmallVector&;
		constexpr auto operator=(std::initializer_list<value_type> rhs) -> SmallVector&;

		NODISC constexpr bool operator==(const SmallVector& rhs) const;

	public:
		NODISC constexpr auto get_allocator() const noexcept -> allocator_type;

		NODISC constexpr auto operator[](size_type pos) -> reference;
		NODISC constexpr auto operator[](size_type pos) const -> const_reference;

		NODISC constexpr auto at(size_type pos) -> reference;
		NODISC constexpr auto at(size_type pos) const -> const_reference;

		NODISC constexpr auto front() -> reference;
		NODISC constexpr auto front() const -> const_reference;

		NODISC constexpr auto back() -> reference;
		NODISC constexpr auto back() const -> const_reference;

		NODISC constexpr auto data() noexcept -> pointer;
		NODISC constexpr auto data() const noexcept -> const_pointer;

		NODISC constexpr bool empty() const noexcept;
		NODISC constexpr auto size() const noexcept -> size_type;

		NODISC constexpr auto max_size() const noexcept -> size_type;

	public:
		constexpr void shrink_to_fit();

		template<typename... Args>
		constexpr auto emplace_back(Args&&... args) -> reference;

		constexpr void push_back(const T& element);
		constexpr void push_back(T&& element);

		constexpr void pop_back();

		constexpr auto erase(const const_iterator it) -> iterator;
		constexpr auto insert(const const_iterator it, const_reference value) -> iterator;

		constexpr void fill(const_reference value);
		constexpr void resize(size_type size, T value = T());

		constexpr void clear();

		constexpr void swap(SmallVector& other) noexcept;

	public:
		NODISC constexpr auto begin() -> iterator;
		NODISC constexpr auto end() -> iterator;

		NODISC constexpr auto begin() const -> const_iterator;
		NODISC constexpr auto end() const -> const_iterator;

		NODISC constexpr auto cbegin() const -> const_iterator;
		NODISC constexpr auto cend() const -> const_iterator;

		NODISC constexpr auto rbegin() -> reverse_iterator;
		NODISC constexpr auto rend() -> reverse_iterator;

		NODISC constexpr auto rbegin() const -> const_reverse_iterator;
		NODISC constexpr auto rend() const -> const_reverse_iterator;

		NODISC constexpr auto crbegin() const -> const_reverse_iterator;
		NODISC constexpr auto crend() const -> const_reverse_iterator;

	private:
		std::array<T, N>		m_stack {T()};
		std::vector<T, Alloc>	m_heap;
		size_type				m_size	{0};
	};

	template<class T, std::size_t N, class Alloc>
	inline constexpr SmallVector<T, N, Alloc>::SmallVector(size_type size, const_reference value, const allocator_type& alloc)
	{
		if (size == N)
		{
			m_stack.fill(value);
		}
		else if (size < N)
		{
			for (std::size_t i = 0; i < size; ++i)
				m_stack[i] = value;
		}
		else
		{
			m_heap = std::move(std::vector<T>(size, value, alloc));
		}

		m_size = size;
	}

	template<class T, std::size_t N, class Alloc>
	template<class Iter> requires std::contiguous_iterator<Iter>
	inline constexpr SmallVector<T, N, Alloc>::SmallVector(Iter first, Iter last, const allocator_type& alloc)
	{
		size_type size = last - first;

		if (size <= N)
		{
			for (Iter it = first; it != last; ++it, ++m_size)
				m_stack[m_size] = *it;
		}
		else
		{
			m_heap = std::move(std::vector<T>(first, last, alloc));
			m_size = size;
		}
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr SmallVector<T, N, Alloc>::SmallVector(const SmallVector& other, const allocator_type& alloc)
		: m_stack(other.m_stack), m_heap(other.m_heap, alloc), m_size(other.m_size) {}

	template<class T, std::size_t N, class Alloc>
	inline constexpr SmallVector<T, N, Alloc>::SmallVector(SmallVector&& other, const allocator_type& alloc) noexcept
		: m_stack(std::move(other.m_stack)), m_heap(std::move(other.m_heap), alloc), m_size(std::move(other.m_size)) {}

	template<class T, std::size_t N, class Alloc>
	inline constexpr SmallVector<T, N, Alloc>::SmallVector(std::initializer_list<value_type> init_list, const allocator_type& alloc)
	{
		const auto input_size = init_list.size();

		if (input_size <= N)
		{
			std::copy(init_list.begin(), init_list.end(), m_stack.begin());
		}
		else
		{
			m_heap = std::move(std::vector<T>(init_list, alloc));
		}

		m_size = input_size;
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::operator=(const SmallVector& rhs) -> SmallVector&
	{
		m_stack = rhs.m_stack;
		m_heap = rhs.m_heap;
		m_size = rhs.m_size;
		return *this;
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::operator=(SmallVector&& rhs) noexcept -> SmallVector&
	{
		m_stack = std::move(rhs.m_stack);
		m_heap = std::move(rhs.m_heap);
		m_size = rhs.m_size;
		rhs.m_size = 0;
		return *this;
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::operator=(std::initializer_list<value_type> rhs) -> SmallVector&
	{
		if (rhs.size() <= N)
			m_stack = rhs;
		else
			m_heap = rhs;

		m_size = rhs.size();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr bool SmallVector<T, N, Alloc>::operator==(const SmallVector& rhs) const
	{
		if (m_size != rhs.m_size)
			return false;

		if (m_size <= N)
		{
			for (size_type i = 0; i < m_size; ++i)
			{
				if (m_stack[i] != rhs[i])
					return false;
			}

			return true;
		}
		
		return (m_heap == rhs.m_heap);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::get_allocator() const noexcept -> allocator_type
	{
		return m_heap.get_allocator();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::operator[](size_type pos) -> reference
	{
		return (m_size <= N) ? m_stack[pos] : m_heap[pos];
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::operator[](size_type pos) const -> const_reference
	{
		return (m_size <= N) ? m_stack[pos] : m_heap[pos];
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::at(size_type pos) -> reference
	{
		return (m_size <= N) ? m_stack.at(pos) : m_heap.at(pos);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::at(size_type pos) const -> const_reference
	{
		return (m_size <= N) ? m_stack.at(pos) : m_heap.at(pos);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::front() -> reference
	{
		return (m_size <= N) ? m_stack.front() : m_heap.front();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::front() const -> const_reference
	{
		return (m_size <= N) ? m_stack.front() : m_heap.front();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::back() -> reference
	{
		return (m_size <= N) ? m_stack[m_size - 1] : m_heap.back();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::back() const -> const_reference
	{
		return (m_size <= N) ? m_stack[m_size - 1] : m_heap.back();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::data() noexcept -> pointer
	{
		return (m_size <= N) ? m_stack.data() : m_heap.data();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::data() const noexcept -> const_pointer
	{
		return (m_size <= N) ? m_stack.data() : m_heap.data();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr bool SmallVector<T, N, Alloc>::empty() const noexcept
	{
		return (m_size == 0);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::size() const noexcept -> size_type
	{
		return m_size;
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::max_size() const noexcept -> size_type
	{
		return (m_size <= N) ? m_stack.max_size() : m_heap.max_size();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr void SmallVector<T, N, Alloc>::shrink_to_fit()
	{
		if (m_size > N)
			m_heap.shrink_to_fit();
	}

	template<class T, std::size_t N, class Alloc>
	template<typename... Args>
	inline constexpr auto SmallVector<T, N, Alloc>::emplace_back(Args&&... args) -> reference
	{
		if (m_size < N)
		{
			m_stack[m_size] = T(std::forward<Args>(args)...);
		}
		else
		{
			if (m_size == N)
				std::move(m_stack.begin(), m_stack.end(), std::back_inserter(m_heap));

			m_heap.emplace_back(std::forward<Args>(args)...);
		}

		++m_size;

		return back();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr void SmallVector<T, N, Alloc>::push_back(const T& element)
	{
		emplace_back(element);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr void SmallVector<T, N, Alloc>::push_back(T&& element)
	{
		emplace_back(std::move(element));
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr void SmallVector<T, N, Alloc>::pop_back()
	{
		if (m_size == 0)
			return;

		if (m_size <= N)
		{
			--m_size;
		}
		else
		{
			m_heap.pop_back();
			--m_size;

			if (m_size == N)
			{
				std::move(m_heap.begin(), m_heap.end(), m_stack.begin());
				m_heap.clear();
			}
		}
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::erase(const const_iterator it) -> iterator
	{
		if (m_size == 0) // nothing to erase
			return end();

		size_type pos = static_cast<size_type>(it - cbegin());

		if (m_size <= N)
		{
			for (auto i = pos; i < m_size - 1; ++i)
				m_stack[i] = std::move(m_stack[i + 1]);

			--m_size;
		}
		else
		{
			m_heap.erase(m_heap.begin() + pos);
			--m_size;

			if (m_size == N)
			{
				std::move(m_heap.begin(), m_heap.end(), m_stack.begin());
				m_heap.clear();
			}
		}

		return iterator(data() + pos);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::insert(const const_iterator it, const_reference value) -> iterator
	{
		size_type pos = static_cast<size_type>(it - cbegin());

		if (m_size < N)
		{
			for (auto i = (int)m_size - 1; i >= (int)pos; --i)
				m_stack[i + 1] = std::move(m_stack[i]);

			m_stack[pos] = value;
		}
		else
		{
			if (m_size == N)
				std::move(m_stack.begin(), m_stack.end(), std::back_inserter(m_heap));

			m_heap.insert(m_heap.begin() + pos, value);
		}

		++m_size;

		return iterator(data() + pos);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr void SmallVector<T, N, Alloc>::fill(const_reference value)
	{
		if (m_size <= N)
			m_stack.fill(value);
		else
			std::fill(m_heap.begin(), m_heap.end(), value);
	}
	template<class T, std::size_t N, class Alloc>
	inline constexpr void SmallVector<T, N, Alloc>::resize(size_type size, T value)
	{
		if (size <= N)
		{
			if (m_size > N) // data is currently on stack, move to heap
				std::move(m_heap.begin(), m_heap.end(), m_stack.begin());
		}
		else
		{
			if (m_size < N)
				std::move(m_stack.begin(), m_stack.end(), std::back_inserter(m_heap));

			m_heap.resize(size, value);
		}
		
		m_size = size;
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr void SmallVector<T, N, Alloc>::clear()
	{
		if (m_size > N)
			m_heap.clear();

		m_size = 0;
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr void SmallVector<T, N, Alloc>::swap(SmallVector& other) noexcept
	{
		std::swap(m_stack, other.m_stack);
		std::swap(m_heap, other.m_heap);
		std::swap(m_size, other.m_size);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::begin() -> iterator
	{
		return (m_size <= N) ? 
			iterator(m_stack.data()) : 
			iterator(m_heap.data());
	}
	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::end() -> iterator
	{
		return (m_size <= N) ? 
			iterator(m_stack.data() + m_size) : 
			iterator(m_heap.data() + m_size);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::begin() const -> const_iterator
	{
		return const_cast<SmallVector&>(*this).begin();
	}
	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::end() const -> const_iterator
	{
		return const_cast<SmallVector&>(*this).end();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::cbegin() const -> const_iterator
	{
		return begin();
	}
	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::cend() const -> const_iterator
	{
		return end();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::rbegin() -> reverse_iterator
	{
		return (m_size <= N) ?
			reverse_iterator(m_stack.data() + m_size - 1) :
			reverse_iterator(m_heap.data() + m_size - 1);
	}
	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::rend() -> reverse_iterator
	{
		return (m_size <= N) ?
			reverse_iterator(m_stack.data() - 1) :
			reverse_iterator(m_heap.data() - 1);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::rbegin() const -> const_reverse_iterator
	{
		return const_cast<SmallVector&>(*this).rbegin();
	}
	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::rend() const -> const_reverse_iterator
	{
		return const_cast<SmallVector&>(*this).rend();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::crbegin() const -> const_reverse_iterator
	{
		return rbegin();
	}
	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::crend() const -> const_reverse_iterator
	{
		return rend();
	}
}