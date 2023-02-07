#pragma once

#include <array>
#include <vector>
#include <iterator>

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
		SmallIterator() noexcept : m_ptr(nullptr) {}
		SmallIterator(pointer ptr) noexcept : m_ptr(ptr) {}

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

		difference_type operator-(const SmallIterator& rhs) const noexcept { return (m_ptr - rhs.m_ptr) * DIR; }

		SmallIterator operator+(const difference_type i) const noexcept { return (SmallIterator(*this) += i * DIR); }
		SmallIterator operator-(const difference_type i) const noexcept { return (SmallIterator(*this) += -i * DIR); }

		reference operator[](const difference_type i) const noexcept { return *(*this + i * DIR); }

		template<class U, int DIR2>
		bool operator==(const SmallIterator<U, DIR2>& rhs) const noexcept { return m_ptr == rhs.m_ptr; }

		template<class U, int DIR2>
		std::strong_ordering operator<=>(const SmallIterator<U, DIR2>& rhs) const noexcept
		{
			return (DIR == 1) ? (m_ptr <=> rhs.m_ptr) : 0 <=> (m_ptr <=> rhs.m_ptr);
		}

		friend SmallIterator operator+(const difference_type lhs, const SmallIterator& rhs)
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

		constexpr explicit SmallVector(const size_type size, const_reference value = value_type(), const allocator_type& alloc = allocator_type());
		template<class iter>
		constexpr SmallVector(iter first, iter last, const allocator_type& alloc = allocator_type());
		constexpr SmallVector(const SmallVector& other, const allocator_type& alloc = allocator_type());
		constexpr SmallVector(SmallVector&& other, const allocator_type& alloc = allocator_type()) noexcept;
		constexpr SmallVector(std::initializer_list<value_type> init_list, const allocator_type& alloc = allocator_type());

		constexpr auto operator=(const SmallVector& rhs) -> SmallVector&;
		constexpr auto operator=(SmallVector&& rhs) noexcept -> SmallVector&;
		constexpr auto operator=(std::initializer_list<value_type> rhs) -> SmallVector&;

		constexpr bool operator==(const SmallVector& rhs) const;
		constexpr bool operator!=(const SmallVector& rhs) const;

	public:
		constexpr auto get_allocator() const noexcept -> allocator_type;

		constexpr auto operator[](const size_type pos) -> reference;
		constexpr auto operator[](const size_type pos) const -> const_reference;

		constexpr auto at(const size_type pos) -> reference;
		constexpr auto at(const size_type pos) const -> const_reference;

		constexpr auto front() -> reference;
		constexpr auto front() const -> const_reference;

		constexpr auto back() -> reference;
		constexpr auto back() const -> const_reference;

		constexpr auto data() noexcept -> pointer;
		constexpr auto data() const noexcept -> const_pointer;

		constexpr bool empty() const noexcept;
		constexpr auto size() const noexcept -> size_type;

		constexpr auto max_size() const noexcept -> size_type;

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
		constexpr void resize(const size_type size, T value = T());

		constexpr void swap(SmallVector& other) noexcept;

	public:
		constexpr auto begin() -> iterator;
		constexpr auto end() -> iterator;

		constexpr auto begin() const -> const_iterator;
		constexpr auto end() const -> const_iterator;

		constexpr auto cbegin() const -> const_iterator;
		constexpr auto cend() const -> const_iterator;

	private:
		std::array<T, N>		m_stack {T()}; // TODO: perhaps switch to using union since both will never be used at the same time
		std::vector<T, Alloc>	m_heap;
		size_type				m_size	{0};
	};

	template<class T, std::size_t N, class Alloc>
	inline constexpr SmallVector<T, N, Alloc>::SmallVector(const size_type size, const_reference value, const allocator_type& alloc)
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
	template<class iter>
	inline constexpr SmallVector<T, N, Alloc>::SmallVector(iter first, iter last, const allocator_type& alloc)
	{
		const size_type size = last - first;

		if (size <= N)
		{
			for (iter it = first; it != last; ++it, ++m_size)
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
	inline constexpr bool SmallVector<T, N, Alloc>::operator!=(const SmallVector& rhs) const
	{
		return !(*this == rhs);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::get_allocator() const noexcept -> allocator_type
	{
		return m_heap.get_allocator();
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::operator[](const size_type pos) -> reference
	{
		return (m_size <= N) ? m_stack[pos] : m_heap[pos];
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::operator[](const size_type pos) const -> const_reference
	{
		return (m_size <= N) ? m_stack[pos] : m_heap[pos];
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::at(const size_type pos) -> reference
	{
		return (m_size <= N) ? m_stack.at(pos) : m_heap.at(pos);
	}

	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::at(const size_type pos) const -> const_reference
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

		auto pos = it - cbegin();

		if (m_size <= N)
		{
			for (auto i = pos; i < (int)m_size - 1; ++i)
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
		auto pos = it - cbegin();

		if (m_size < N)
		{
			for (auto i = (int)m_size - 1; i >= pos; --i)
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
	inline constexpr void SmallVector<T, N, Alloc>::resize(const size_type size, T value)
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
		return (m_size <= N) ?
			const_iterator(m_stack.data()) : 
			const_iterator(m_heap.data());
	}
	template<class T, std::size_t N, class Alloc>
	inline constexpr auto SmallVector<T, N, Alloc>::end() const -> const_iterator
	{
		return (m_size <= N) ?
			const_iterator(m_stack.data() + m_size) : 
			const_iterator(m_heap.data() + m_size);
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
}