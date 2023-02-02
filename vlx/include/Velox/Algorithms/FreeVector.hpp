#pragma once

#include <vector>

#include <Velox/Utilities/Concepts.h>

namespace vlx
{
	template<class T>
	class FreeVector
	{
	private:
		using size_type = std::int32_t;

	public:
		FreeVector() = default;

	public:
		T& operator[](const size_type n);
		const T& operator[](const size_type n) const;

	public:
		constexpr auto Insert(const T& element) -> size_type;
		void Erase(const size_type n);

		void Clear();

		constexpr auto Range() const -> size_type;

	private:
		union FreeElement
		{
			T element;
			size_type next;
		};

		std::vector<FreeElement> m_data;
		size_type m_first_free {-1};
	};

	template<class T>
	inline T& FreeVector<T>::operator[](const size_type n)
	{
		return m_data[n].element;
	}

	template<class T>
	inline const T& FreeVector<T>::operator[](const size_type n) const
	{
		return m_data[n].element;
	}

	template<class T>
	inline constexpr auto FreeVector<T>::Insert(const T& element) -> size_type
	{
		if (m_first_free != -1)
		{
			const int index = m_first_free;
			m_first_free = m_data[m_first_free].next;
			m_data[index].element = element;

			return index;
		}
		else
		{
			FreeElement free_element{};
			free_element.element = element;
			m_data.push_back(free_element);

			return static_cast<size_type>(m_data.size() - 1);
		}
	}

	template<class T>
	inline void FreeVector<T>::Erase(const size_type n)
	{
		m_data[n].next = m_first_free;
		m_first_free = n;
	}

	template<class T>
	inline void FreeVector<T>::Clear()
	{
		m_data.clear();
		m_first_free = -1;
	}

	template<class T>
	inline constexpr auto FreeVector<T>::Range() const -> size_type
	{
		return static_cast<size_type>(m_data.size());
	}
}