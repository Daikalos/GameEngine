#pragma once

#include <SFML/Graphics.hpp>

#include <vector>
#include <unordered_map>
#include <concepts>
#include <functional>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>

#include "State.h"

namespace vlx
{
	class VELOX_API StateStack : private NonCopyable
	{
	private:
		using Stack = typename std::vector<State::Ptr>;
		using Factory = typename std::unordered_map<State::ID, State::Func>;

		enum class Action
		{
			Push,
			Pop,
			Erase,
			Clear
		};

		struct PendingChange
		{
			explicit PendingChange(const Action& action, const State::ID state_id = -1)
				: action(action), state_id(state_id) { }

			const Action	action;
			const State::ID	state_id;
		};

	public:
		explicit StateStack(State::Context context);

		template<std::derived_from<State> T, typename... Args>
		void RegisterState(const State::ID state_id, Args&&... args);

		void HandleEvent(const sf::Event& event);

		void PreUpdate(Time& time);
		void Update(Time& time);
		void FixedUpdate(Time& time);
		void PostUpdate(Time& time);

		void Draw();

		void Push(const State::ID state_id);
		void Pop();
		void Clear();

		[[nodiscard]] bool IsEmpty() const noexcept;

		[[nodiscard]] bool IsPaused() const noexcept;
		void SetPaused(bool flag);

	private:
		State::Ptr CreateState(const State::ID state_id);
		void ApplyPendingChanges();

	private:
		State::Context				m_context;

		Stack						m_stack;		// all states are stored here
		Factory						m_factory;		// factory for storing functions that creates the registered object
		std::vector<PendingChange>	m_pending_list; // pending list of changes to the stack, applied last in the update

		bool m_paused{false};
	};

	template<std::derived_from<State> T, typename... Args>
	inline void StateStack::RegisterState(const State::ID state_id, Args&&... args)
	{
		m_factory[state_id] = [&state_id, this, &args...]()
		{
			return State::Ptr(new T(state_id, *this, m_context, std::forward<Args>(args)...));
		};
	}
}