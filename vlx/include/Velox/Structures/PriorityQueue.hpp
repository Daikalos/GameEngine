#pragma once

#include <vector>
#include <algorithm>

#include <Velox/Config.hpp>

namespace vlx
{
	namespace pq
	{
		enum class Comparison : bool
		{
			Less	= false,
			Greater	= true
		};
	}

	/// Custom container to allow modifying the elements while still maintaing order due to separating the 
	/// priority from the item itself.
	/// 
	template<typename T, pq::Comparison C = pq::Comparison::Less>
	class PriorityQueue
	{
	public:
		using value_type		= T;
		using reference			= T&;
		using const_reference	= const T&;
		using size_type			= std::size_t;

	private:
		struct Node
		{
			value_type	item;
			float		priority {0.0f};
		};

		using container = std::vector<Node>;

	public:
		using iterator					= typename container::iterator;
		using const_iterator			= typename container::const_iterator;
		using reverse_iterator			= typename container::reverse_iterator;
		using const_reverse_iterator	= typename container::const_reverse_iterator;

	private:
		template<pq::Comparison>
		class Comp {};

		template<>
		struct Comp<pq::Comparison::Less>
		{
			NODISC constexpr bool operator()(const Node& lhs, const Node& rhs) const noexcept
			{
				return lhs.priority < rhs.priority;
			}
		};

		template<>
		struct Comp<pq::Comparison::Greater>
		{
			NODISC constexpr bool operator()(const Node& lhs, const Node& rhs) const noexcept
			{
				return lhs.priority > rhs.priority;
			}
		};

	public:
		PriorityQueue() = default;

		// TODO: implement more constructors

	public:
		NODISC constexpr auto operator[](size_type n) -> reference;
		NODISC constexpr auto operator[](size_type n) const -> const_reference;

		NODISC constexpr auto at(size_type n) -> reference;
		NODISC constexpr auto at(size_type n) const -> const_reference;

		NODISC constexpr bool empty() const noexcept;
		NODISC constexpr auto size() const noexcept -> size_type;

		NODISC constexpr auto max_size() const noexcept -> size_type;

		NODISC constexpr auto top() noexcept -> reference;
		NODISC constexpr auto top() const noexcept -> const_reference;

	public:
		constexpr void push(const T& item, float priority);
		constexpr void push(T&& item, float priority);

		template<typename... Args> requires std::constructible_from<T, Args...>
		constexpr void emplace(float priority, Args&&... args);
		
		constexpr void pop();

		constexpr void reserve(size_type capacity);

		constexpr void clear();

		constexpr void shrink_to_fit();

	public:
		NODISC constexpr auto begin() const -> const_iterator;
		NODISC constexpr auto end() const -> const_iterator;

		NODISC constexpr auto cbegin() const -> const_iterator;
		NODISC constexpr auto cend() const -> const_iterator;

		NODISC constexpr auto rbegin() const -> const_reverse_iterator;
		NODISC constexpr auto rend() const -> const_reverse_iterator;

		NODISC constexpr auto crbegin() const -> const_reverse_iterator;
		NODISC constexpr auto crend() const -> const_reverse_iterator;

	private:
		container	m_nodes;
		Comp<C>		m_comp;
	};

	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::operator[](size_type n) -> reference
	{
		return m_nodes[n].item;
	}
	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::operator[](size_type n) const -> const_reference
	{
		return m_nodes[n].item;
	}

	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::at(size_type n) -> reference
	{
		return m_nodes.at(n).item;
	}
	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::at(size_type n) const -> const_reference
	{
		return m_nodes.at(n).item;
	}

	template<typename T, pq::Comparison C>
	inline constexpr bool PriorityQueue<T, C>::empty() const noexcept
	{
		return m_nodes.empty();
	}
	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::size() const noexcept -> size_type
	{
		return m_nodes.size();
	}

	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::max_size() const noexcept -> size_type
	{
		return m_nodes.max_size();
	}

	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::top() noexcept -> reference
	{
		return m_nodes.front().item;
	}
	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::top() const noexcept -> const_reference
	{
		return m_nodes.front().item;
	}

	template<typename T, pq::Comparison C>
	inline constexpr void PriorityQueue<T, C>::push(const T& item, float priority)
	{
		emplace(priority, item);
	}
	template<typename T, pq::Comparison C>
	inline constexpr void PriorityQueue<T, C>::push(T&& item, float priority)
	{
		emplace(priority, std::move(item));
	}

	template<typename T, pq::Comparison C>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline constexpr void PriorityQueue<T, C>::emplace(float priority, Args && ...args)
	{
		m_nodes.emplace_back(T(std::forward<Args>(args)...), priority);
		std::ranges::push_heap(m_nodes.begin(), m_nodes.end(), m_comp);
	}

	template<typename T, pq::Comparison C>
	inline constexpr void PriorityQueue<T, C>::pop()
	{
		std::ranges::pop_heap(m_nodes.begin(), m_nodes.end(), m_comp);
		m_nodes.pop_back();
	}

	template<typename T, pq::Comparison C>
	inline constexpr void PriorityQueue<T, C>::reserve(size_type capacity)
	{
		m_nodes.reserve(capacity);
	}

	template<typename T, pq::Comparison C>
	inline constexpr void PriorityQueue<T, C>::clear()
	{
		m_nodes.clear();
	}

	template<typename T, pq::Comparison C>
	inline constexpr void PriorityQueue<T, C>::shrink_to_fit()
	{
		m_nodes.shrink_to_fit();
	}

	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::begin() const -> const_iterator
	{
		return m_nodes.begin();
	}
	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::end() const -> const_iterator
	{
		return m_nodes.begin();
	}

	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::cbegin() const -> const_iterator
	{
		return m_nodes.cbegin();
	}
	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::cend() const -> const_iterator
	{
		return m_nodes.cbegin();
	}

	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::rbegin() const -> const_reverse_iterator
	{
		return m_nodes.rbegin();
	}
	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::rend() const -> const_reverse_iterator
	{
		return m_nodes.rbegin();
	}

	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::crbegin() const -> const_reverse_iterator
	{
		return m_nodes.crbegin();
	}
	template<typename T, pq::Comparison C>
	inline constexpr auto PriorityQueue<T, C>::crend() const -> const_reverse_iterator
	{
		return m_nodes.crbegin();
	}
}