#pragma once

#include <shared_mutex>
#include <vector>
#include <future>

#include <Velox/Config.hpp>

#include "EventHandler.hpp"
#include "IEvent.hpp"

namespace vlx
{
	/// Can hold any number of functions that may all be executed manually.
	/// 
	/// Based on article by Shmuel Zang:
	/// https://www.codeproject.com/Articles/1256352/CppEvent-How-to-Implement-Events-using-Standard-Cp
	/// 
	/// \param Args: Parameters for function
	/// 
	template<typename... Args>
	class Event : public IEvent
	{
	public:
		using HandlerType = EventHandler<Args...>;

	protected:
		using HandlerList = std::vector<HandlerType>;

	public:
		Event() = default;

		Event(const Event& other);
		Event(Event&& other) noexcept;

		auto operator=(const Event& other) -> Event&;
		auto operator=(Event&& other) noexcept -> Event&;

		explicit operator bool() const;

		void operator()(Args... params) const;

		evnt::IDType operator+=(const HandlerType& handler);
		evnt::IDType operator+=(const typename HandlerType::FuncType& handler);

		evnt::IDType operator-=(const HandlerType& handler);
		evnt::IDType operator-=(evnt::IDType handler_id) override;

	public:
		NODISC std::size_t Count() const noexcept override;
		NODISC bool IsEmpty() const noexcept override;

		void Reserve(std::size_t size) override;
		void Clear() noexcept override;

		evnt::IDType Add(const HandlerType& handler);
		evnt::IDType Add(const typename HandlerType::FuncType& handler);

		bool Remove(const HandlerType& handler);
		bool RemoveID(evnt::IDType handler_id) override;

		void Call(Args... params) const;
		void CallMain(Args... params) const;
		std::future<void> CallAsync(Args... params) const;

	private:
		void CallImpl(const HandlerList& handlers, Args... params) const;
		auto GetHandlersCopy() const -> HandlerList;

	private:
		HandlerList					m_handlers;
		mutable std::shared_mutex	m_lock;
	};

	template<typename... Args>
	inline std::size_t Event<Args...>::Count() const noexcept
	{
		return m_handlers.size();
	}
	template<typename... Args>
	inline bool Event<Args...>::IsEmpty() const noexcept
	{
		return m_handlers.empty();
	}

	template<typename... Args>
	inline void Event<Args...>::Reserve(std::size_t size)
	{
		m_handlers.reserve(size);
	}

	template<typename... Args>
	inline void Event<Args...>::Clear() noexcept
	{
		m_handlers.clear();
	}

	template<typename... Args>
	inline Event<Args...>::Event(const Event& other)
	{
		std::shared_lock lock(other.m_lock);
		m_handlers = other.m_handlers;
	}
	template<typename... Args>
	inline Event<Args...>::Event(Event&& other) noexcept
	{
		std::lock_guard lock(other.m_lock);
		m_handlers = std::move(other.m_handlers);
	}

	template<typename... Args>
	inline auto Event<Args...>::operator=(const Event& other) -> Event&
	{
		std::lock_guard lock1(m_lock);
		std::shared_lock lock2(other.m_lock);

		m_handlers = other.m_handlers;

		return *this;
	}
	template<typename... Args>
	inline auto Event<Args...>::operator=(Event&& other) noexcept -> Event&
	{
		std::lock_guard lock1(m_lock);
		std::lock_guard lock2(other.m_lock);

		m_handlers = std::move(other.m_handlers);

		return *this;
	}

	template<typename ...Args>
	inline Event<Args...>::operator bool() const
	{
		return !IsEmpty();
	}

	template<typename... Args>
	inline void Event<Args...>::operator()(Args... params) const
	{
		Call(params...);
	}

	template<typename... Args>
	inline evnt::IDType Event<Args...>::operator+=(const HandlerType& handler)
	{
		return Add(handler);
	}
	template<typename... Args>
	inline evnt::IDType Event<Args...>::operator+=(const typename HandlerType::FuncType& handler)
	{
		return Add(handler);
	}

	template<typename... Args>
	inline evnt::IDType Event<Args...>::operator-=(const HandlerType& handler)
	{
		return Remove(handler);
	}
	template<typename... Args>
	inline evnt::IDType Event<Args...>::operator-=(evnt::IDType handler_id)
	{
		return RemoveID(handler_id);
	}

	template<typename... Args>
	inline evnt::IDType Event<Args...>::Add(const HandlerType& handler)
	{
		std::lock_guard lock(m_lock);

		m_handlers.push_back(handler);

		return handler.GetID();
	}
	template<typename... Args>
	inline evnt::IDType Event<Args...>::Add(const typename HandlerType::FuncType& handler)
	{
		return Add(HandlerType(handler));
	}

	template<typename... Args>
	inline bool Event<Args...>::Remove(const HandlerType& handler)
	{
		std::lock_guard lock(m_lock);

		const auto it = std::find(m_handlers.begin(), m_handlers.end(), handler);

		if (it == m_handlers.end())
			return false;

		m_handlers.erase(it);

		return true;
	}
	template<typename... Args>
	inline bool Event<Args...>::RemoveID(evnt::IDType handler_id)
	{
		std::lock_guard lock(m_lock);

		const auto it = std::find_if(m_handlers.begin(), m_handlers.end(),
			[handler_id](const HandlerType& handler)
			{
				return handler.GetID() == handler_id;
			});

		if (it == m_handlers.end())
			return false;

		m_handlers.erase(it);

		return true;
	}

	template<typename... Args>
	inline void Event<Args...>::Call(Args... params) const
	{
		HandlerList handlers_copy = GetHandlersCopy();
		CallImpl(handlers_copy, params...);
	}
	template<typename... Args>
	inline void Event<Args...>::CallMain(Args... params) const
	{
		std::shared_lock lock(m_lock);
		CallImpl(m_handlers, params...); // if called in main thread
	}

	template<typename... Args>
	inline void Event<Args...>::CallImpl(const HandlerList& handlers, Args... params) const
	{
		for (const auto& handler : handlers)
			handler(params...);
	}
	template<typename... Args>
	inline std::future<void> Event<Args...>::CallAsync(Args... params) const
	{
		return std::async(std::launch::async,
			[this](Args... async_params)
			{
				Call(async_params...);
			}, params...);
	}

	template<typename... Args>
	inline auto Event<Args...>::GetHandlersCopy() const -> HandlerList
	{
		std::shared_lock lock(m_lock);
		return m_handlers;
	}
}