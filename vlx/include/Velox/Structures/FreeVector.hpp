#pragma once

#include <vector>
#include <variant>
#include <functional>

#include <Velox/System/Concepts.h>
#include <Velox/Types.hpp>

namespace vlx
{
	// TODO: add docs

	template<class T>
	class FreeVector
	{
	private:
		using value_type		= T;
		using reference			= T&;
		using const_reference	= const T&;
		using pointer			= T*;
		using const_pointer		= const T*;
		using size_type			= std::size_t;

	public:
		FreeVector() = default;

	public:
		NODISC constexpr auto operator[](size_type n) -> reference;
		NODISC constexpr auto operator[](size_type n) const -> const_reference;

		NODISC constexpr auto at(size_type n) -> reference;
		NODISC constexpr auto at(size_type n) const -> const_reference;

		NODISC constexpr bool valid(size_type n) const noexcept;

		NODISC constexpr bool empty() const noexcept;
		NODISC constexpr auto size() const noexcept -> size_type;
		NODISC constexpr auto count() const noexcept -> size_type;

		NODISC constexpr auto max_size() const noexcept -> size_type;

	public:
		template<typename... Args> requires std::constructible_from<T, Args...>
		constexpr auto emplace(Args&&... args) -> size_type;

		constexpr auto insert(const T& element) -> size_type;
		constexpr auto insert(T&& element) -> size_type;

		constexpr void erase(size_type n);

		constexpr void clear();

		constexpr void reserve(size_type capacity);

		constexpr void resize(size_type size);

	public:
		template<typename Func>
		void for_each(Func&& func) const;

	private:
		std::vector<std::variant<T, int64>> m_data;
		int64		m_first_free {-1};
		size_type	m_count		 {0};
	};

	template<class T>
	inline constexpr auto FreeVector<T>::operator[](size_type n) -> reference
	{
		return std::get<T>(m_data[n]);
	}

	template<class T>
	inline constexpr auto FreeVector<T>::operator[](size_type n) const -> const_reference
	{
		return std::get<T>(m_data[n]);
	}

	template<class T>
	inline constexpr auto FreeVector<T>::at(size_type n) -> reference
	{
		return std::get<T>(m_data.at(n));
	}

	template<class T>
	inline constexpr auto FreeVector<T>::at(size_type n) const -> const_reference
	{
		return std::get<T>(m_data.at(n));
	}

	template<class T>
	inline constexpr bool FreeVector<T>::valid(size_type n) const noexcept
	{
		return m_data.at(n).index() == 0;
	}

	template<class T>
	inline constexpr bool FreeVector<T>::empty() const noexcept
	{
		return m_count == 0;
	}

	template<class T>
	inline constexpr auto FreeVector<T>::size() const noexcept -> size_type
	{
		return static_cast<size_type>(m_data.size());
	}

	template<class T>
	inline constexpr auto FreeVector<T>::count() const noexcept -> size_type
	{
		return m_count;
	}

	template<class T>
	inline constexpr auto FreeVector<T>::max_size() const noexcept -> size_type
	{
		return static_cast<size_type>(m_data.max_size());
	}

	template<class T>
	template<typename... Args> requires std::constructible_from<T, Args...>
	inline constexpr auto FreeVector<T>::emplace(Args&&... args) -> size_type
	{
		size_type index = 0;

		if (m_first_free != -1)
		{
			assert(!valid(m_first_free));

			index = m_first_free;

			m_first_free	= std::get<int64>(m_data[m_first_free]);
			m_data[index]	= { std::in_place_index<0>, std::forward<Args>(args)... };
		}
		else
		{
			assert(m_data.size() == m_count); // should be the same if no more available space

			index = m_data.size();
			m_data.emplace_back(std::in_place_index<0>, std::forward<Args>(args)...);
		}

		++m_count;

		return index;
	}

	template<class T>
	inline constexpr auto FreeVector<T>::insert(const T& element) -> size_type
	{
		return emplace(element);
	}

	template<class T>
	inline constexpr auto FreeVector<T>::insert(T&& element) -> size_type
	{
		return emplace(std::move(element));
	}

	template<class T>
	inline constexpr void FreeVector<T>::erase(size_type n)
	{
		assert(n >= 0 && n < m_data.size() && valid(n));

		m_data[n] = m_first_free;
		m_first_free = n;

		--m_count;
	}

	template<class T>
	inline constexpr void FreeVector<T>::clear()
	{
		m_data.clear();
		m_first_free = -1;
		m_count = 0;
	}

	template<class T>
	inline constexpr void FreeVector<T>::reserve(size_type capacity)
	{
		m_data.reserve(capacity);
	}

	template<class T>
	inline constexpr void FreeVector<T>::resize(size_type size)
	{
		if (size >= m_count)
		{
			std::size_t diff = size - m_count;
			for (std::size_t i = 0; i < diff; ++i)
				emplace(T{});
		}
		else
		{
			assert(false);
			// TODO: implement ???
		}

		m_count = size;
	}

	template<class T>
	template<typename Func>
	inline void FreeVector<T>::for_each(Func&& func) const
	{
		std::for_each(m_data.begin(), m_data.end(),
			[func = std::forward<Func>(func)](const auto& elt)
			{
				if (elt.index() == 0)
					func(std::get<T>(elt));
			});
	}
}