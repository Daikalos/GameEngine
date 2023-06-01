#pragma once

#include <vector>
#include <variant>
#include <functional>

#include <Velox/System/Concepts.h>
#include <Velox/VeloxTypes.hpp>

namespace vlx
{
	template<class T>
	class FreeVector
	{
	private:
		using value_type		= T;
		using reference			= T&;
		using const_reference	= const T&;
		using pointer			= T*;
		using const_pointer		= const T*;
		using size_type			= int64;

	public:
		FreeVector() = default;

	public:
		NODISC constexpr auto operator[](const size_type n) -> reference;
		NODISC constexpr auto operator[](const size_type n) const -> const_reference;

		NODISC constexpr auto at(const size_type n) -> reference;
		NODISC constexpr auto at(const size_type n) const -> const_reference;

		NODISC constexpr bool valid(const size_type n) const noexcept;

		NODISC constexpr bool empty() const noexcept;
		NODISC constexpr auto size() const noexcept -> size_type;

		NODISC constexpr auto max_size() const noexcept -> size_type;

	public:
		template<typename... Args> requires std::constructible_from<T, Args...>
		constexpr auto emplace(Args&&... args) -> size_type;

		constexpr auto insert(const T& element) -> size_type;
		constexpr auto insert(T&& element) -> size_type;

		constexpr void erase(const size_type n);

		constexpr void clear();

	public:
		void for_each(const std::function<void(reference)>& func) const;

	private:
		std::vector<std::variant<T, size_type>> m_data;
		size_type m_first_free {-1};
	};

	template<class T>
	inline constexpr auto FreeVector<T>::operator[](const size_type n) -> reference
	{
		return std::get<T>(m_data[n]);
	}

	template<class T>
	inline constexpr auto FreeVector<T>::operator[](const size_type n) const -> const_reference
	{
		return std::get<T>(m_data[n]);
	}

	template<class T>
	inline constexpr auto FreeVector<T>::at(const size_type n) -> reference
	{
		return std::get<T>(m_data.at(n));
	}

	template<class T>
	inline constexpr auto FreeVector<T>::at(const size_type n) const -> const_reference
	{
		return std::get<T>(m_data.at(n));
	}

	template<class T>
	inline constexpr bool FreeVector<T>::valid(const size_type n) const noexcept
	{
		return m_data.at(n).index() == 0;
	}

	template<class T>
	inline constexpr bool FreeVector<T>::empty() const noexcept
	{
		return m_data.empty();
	}

	template<class T>
	inline constexpr auto FreeVector<T>::size() const noexcept -> size_type
	{
		return static_cast<size_type>(m_data.size());
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
		if (m_first_free != -1)
		{
			assert(!valid(m_first_free));

			const auto index = m_first_free;
			m_first_free = std::get<size_type>(m_data[m_first_free]);
			m_data[index] = { std::in_place_index<0>, T(std::forward<Args>(args)...) };

			return index;
		}
		else
		{
			m_data.emplace_back(std::in_place_index<0>, std::forward<Args>(args)...);
			return static_cast<size_type>(m_data.size() - 1);
		}
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
	inline constexpr void FreeVector<T>::erase(const size_type n)
	{
		assert(n >= 0 && n < size());
		m_data[n] = m_first_free;
		m_first_free = n;
	}

	template<class T>
	inline constexpr void FreeVector<T>::clear()
	{
		m_data.clear();
		m_first_free = -1;
	}

	template<class T>
	inline void FreeVector<T>::for_each(const std::function<void(reference)>& func) const
	{
		for (const auto& elt : m_data)
		{
			if (elt.index() == 0)
				func(std::get<T>(elt));
		}
	}
}