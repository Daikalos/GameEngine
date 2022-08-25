#pragma once

#include <SFML/Graphics.hpp>

#include <vector>
#include <unordered_map>
#include <concepts>
#include <functional>

#include "State.hpp"
#include "States.h"

#include "../utilities/NonCopyable.h"
#include "../utilities/Time.hpp"

namespace vlx
{
	class StateStack : private NonCopyable
	{
	private:
		using Stack = typename std::vector<State::Ptr>;
		using Factory = typename std::unordered_map<state::ID, State::Func>;

		enum class Action
		{
			Push,
			Pop,
			Erase,
			Clear
		};

		struct PendingChange
		{
			explicit PendingChange(const Action& action, const state::ID& state_id = state::ID::None)
				: action(action), state_id(state_id) { }

			const Action		action;
			const state::ID		state_id;
		};

	public:
		explicit StateStack(State::Context context);

		template<std::derived_from<State> T, typename... Args>
		void RegisterState(const state::ID& state_id, Args&&... args);

		void HandleEvent(const sf::Event& event);

		void PreUpdate(Time& time);
		void Update(Time& time);
		void FixedUpdate(Time& time);
		void PostUpdate(Time& time, float interp);

		void Draw();

		void Push(const state::ID& state_id);
		void Pop();
		void Clear();

		bool IsEmpty() const { return m_stack.empty(); }

		bool IsPaused() const { return m_paused; }
		void SetPaused(bool flag) { m_paused = flag; }

	private:
		State::Ptr CreateState(const state::ID& state_id);
		void ApplyPendingChanges();

	private:
		State::Context				m_context;

		Stack						m_stack;		// all states are stored here
		Factory						m_factory;		// factory for storing functions that creates the registered object
		std::vector<PendingChange>	m_pending_list; // pending list of changes to the stack, applied last in the update

		bool m_paused{false};
	};

	template<std::derived_from<State> T, typename... Args>
	inline void StateStack::RegisterState(const state::ID& state_id, Args&&... args)
	{
		m_factory[state_id] = [&state_id, this, &args...]()
		{
			return State::Ptr(new T(state_id, *this, m_context, std::forward<Args>(args)...));
		};
	}
}