#pragma once

#include <functional>
#include <atomic>

namespace vlx
{
	////////////////////////////////////
	// 
	// Event & EventHandler based on article by Shmuel Zang:
	// https://www.codeproject.com/Articles/1256352/CppEvent-How-to-Implement-Events-using-Standard-Cp
	// 
	////////////////////////////////////

	template<typename... Args>
	class EventHandler
	{
	public:
		using FuncType	= std::function<void(Args...)>;
		using IDType	= std::uint32_t;

	private:
		using AtomicID	= std::atomic<IDType>;

	public:
		explicit EventHandler(const FuncType& func);

		EventHandler(const EventHandler& other);
		EventHandler(EventHandler&& other) noexcept;

		auto operator=(const EventHandler& other) -> EventHandler&;
		auto operator=(EventHandler&& other) -> EventHandler&;

		void operator()(Args... params) const;

		bool operator==(const EventHandler& other) const;
		bool operator!=(const EventHandler& other) const;

	public:
		auto GetID() const noexcept -> IDType;

	private:
		FuncType		m_func;

		IDType			m_id;
		static AtomicID m_id_counter;
	};

	template<typename... Args>
	inline std::atomic_uint32_t EventHandler<Args...>::m_id_counter = 1; // 0 reserved for null

	template<typename... Args>
	inline EventHandler<Args...>::EventHandler(const FuncType& func)
		: m_func(func), m_id(m_id_counter++) { }

	template<typename... Args>
	inline EventHandler<Args...>::EventHandler(const EventHandler& other)
		: m_func(other.m_func), m_id(other.m_id) { }

	template<typename... Args>
	inline EventHandler<Args...>::EventHandler(EventHandler&& other) noexcept
		: m_func(std::move(other.m_func)), m_id(other.m_id)
	{
		other.m_id = NULL;
	}

	template<typename... Args>
	inline auto EventHandler<Args...>::operator=(const EventHandler& other) -> EventHandler&
	{
		m_func = other.m_func;
		m_id = other.m_id;

		return *this;
	}

	template<typename... Args>
	inline auto EventHandler<Args...>::operator=(EventHandler&& other) -> EventHandler&
	{
		m_func = std::move(other.m_func);

		m_id = other.m_id;
		other.m_id = NULL;

		return *this;
	}

	template<typename... Args>
	inline void EventHandler<Args...>::operator()(Args... params) const
	{
		if (m_func)
			m_func(params...);
	}

	template<typename... Args>
	inline bool EventHandler<Args...>::operator==(const EventHandler& other) const
	{
		return m_id == other.m_id;
	}
	template<typename... Args>
	inline bool EventHandler<Args...>::operator!=(const EventHandler& other) const
	{
		return !(*this == other);
	}

	template<typename... Args>
	inline auto EventHandler<Args...>::GetID() const noexcept -> IDType
	{
		return m_id;
	}
}