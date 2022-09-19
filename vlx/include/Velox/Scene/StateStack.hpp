#pragma once

#include <SFML/Graphics.hpp>

#include <vector>
#include <unordered_map>
#include <concepts>
#include <functional>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "State.hpp"

namespace vlx
{
	template<IntEnum ID = std::uint16_t>
	class StateStack final : private NonCopyable
	{
	private:
		using State		= typename State<ID>;

		using Stack		= typename std::vector<typename State::Ptr>;
		using Factory	= typename std::unordered_map<ID, typename State::Func>;

		enum class Action
		{
			Push,
			Pop,
			Erase,
			Clear
		};

		struct PendingChange
		{
			explicit PendingChange(const Action& action, const ID state_id = 0);

			const Action	action;
			const ID		state_id;
		};

	public:
		explicit StateStack(State::Context context);

		[[nodiscard]] bool IsEmpty() const noexcept;
		[[nodiscard]] bool IsPaused() const noexcept;

		void SetPaused(bool flag);

	public:
		template<class T, typename... Args>
		void RegisterState(const ID state_id, Args&&... args) requires std::derived_from<T, typename StateStack<ID>::State>;

		void HandleEvent(const sf::Event& event);

		void PreUpdate(Time& time);
		void Update(Time& time);
		void FixedUpdate(Time& time);
		void PostUpdate(Time& time);

		void Draw();

		void Push(const ID state_id);
		void Pop();
		void Clear();

	private:
		auto CreateState(const ID state_id) -> typename State::Ptr;
		void ApplyPendingChanges();

	private:
		State::Context				m_context;

		Stack						m_stack;		// all states are stored here
		Factory						m_factory;		// factory for storing functions that creates the registered object
		std::vector<PendingChange>	m_pending_list; // pending list of changes to the stack, applied last in the update

		bool m_paused{false};
	};

	template<IntEnum ID>
	template<class T, typename... Args>
	inline void StateStack<ID>::RegisterState(const ID state_id, Args&&... args) requires std::derived_from<T, typename StateStack<ID>::State>
	{
		m_factory[state_id] = [this, &state_id, &args...]()
		{
			return typename State::Ptr(new T(state_id, *this, m_context, std::forward<Args>(args)...));
		};
	}

	template<IntEnum ID>
	inline StateStack<ID>::PendingChange::PendingChange(const Action& action, const ID state_id)
		: action(action), state_id(state_id) { }

	template<IntEnum ID>
	inline StateStack<ID>::StateStack(State::Context context)
		: m_context(context) { }

	template<IntEnum ID>
	inline bool StateStack<ID>::IsEmpty() const noexcept
	{
		return m_stack.empty();
	}
	template<IntEnum ID>
	inline bool StateStack<ID>::IsPaused() const noexcept
	{
		return m_paused;
	}

	template<IntEnum ID>
	inline void StateStack<ID>::SetPaused(bool flag)
	{
		m_paused = flag;
	}

	template<IntEnum ID>
	inline void StateStack<ID>::HandleEvent(const sf::Event& event)
	{
		for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
		{
			if (!(*it)->HandleEvent(event))
				break;
		}

		ApplyPendingChanges();
	}

	template<IntEnum ID>
	inline void StateStack<ID>::PreUpdate(Time& time)
	{
		for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
		{
			if (!(*it)->PreUpdate(time))
				break;
		}

		ApplyPendingChanges();
	}

	template<IntEnum ID>
	inline void StateStack<ID>::Update(Time& time)
	{
		for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
		{
			if (!(*it)->Update(time))
				break;
		}

		ApplyPendingChanges();
	}

	template<IntEnum ID>
	inline void StateStack<ID>::FixedUpdate(Time& time)
	{
		for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
		{
			if (!(*it)->FixedUpdate(time))
				break;
		}

		ApplyPendingChanges();
	}

	template<IntEnum ID>
	inline void StateStack<ID>::PostUpdate(Time& time)
	{
		for (auto it = m_stack.rbegin(); it != m_stack.rend(); ++it)
		{
			if (!(*it)->PostUpdate(time))
				break;
		}

		ApplyPendingChanges();
	}

	template<IntEnum ID>
	inline void StateStack<ID>::Draw()
	{
		for (typename State::Ptr& state : m_stack)
			state->draw();
	}

	template<IntEnum ID>
	inline void StateStack<ID>::Push(const ID state_id)
	{
		m_pending_list.push_back(PendingChange(Action::Push, state_id));
	}
	template<IntEnum ID>
	inline void StateStack<ID>::Pop()
	{
		m_pending_list.push_back(PendingChange(Action::Pop));
	}
	template<IntEnum ID>
	inline void StateStack<ID>::Clear()
	{
		m_pending_list.push_back(PendingChange(Action::Clear));
	}

	template<IntEnum ID>
	inline auto StateStack<ID>::CreateState(const ID state_id) -> typename State::Ptr
	{
		auto found = m_factory.find(state_id);
		assert(found != m_factory.end());

		return found->second();
	}

	template<IntEnum ID>
	inline void StateStack<ID>::ApplyPendingChanges()
	{
		for (const PendingChange& change : m_pending_list)
		{
			switch (change.action)
			{
			case Action::Push:
				m_stack.push_back(CreateState(change.state_id));
				break;
			case Action::Pop:
			{
				m_stack.back()->OnDestroy();
				m_stack.pop_back();

				if (!m_stack.empty())
					m_stack.back()->OnActivate();
			}
			break;
			case Action::Clear:
			{
				for (typename State::Ptr& state : m_stack)
					state->OnDestroy();

				m_stack.clear();
			}
			break;
			}
		}

		m_pending_list.clear();
	}
}