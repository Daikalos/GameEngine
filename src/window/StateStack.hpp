#pragma once

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics.hpp>

#include <stack>
#include <vector>
#include <unordered_map>
#include <functional>

#include "State.h"
#include "States.h"

namespace fge
{
	class StateStack : private sf::NonCopyable
	{
	public:
		enum Action
		{
			Push,
			Pop,
			Clear
		};

	public:
		explicit StateStack(State::Context context)
			: _context(context) { }

		template<typename T, typename... Args>
		void register_state(const States::ID& state_id, const Args&&... args);

		void update(const float& dt);
		void draw();
		void handle_event(const sf::Event& event);

		void push(const States::ID& state_id);
		void pop();
		void clear();

		bool is_empty() const
		{
			return _stack.empty();
		}
		bool is_paused() const
		{
			return _is_paused;
		}
		void set_is_paused(bool flag)
		{
			_is_paused = flag;
		}

	private:
		State::ptr create_state(const States::ID& state_id);
		void apply_pending_changes();

	private:
		struct PendingChange
		{
			explicit PendingChange(const Action& action, const States::ID& state_id = States::None)
				: _action(action), _state_id(state_id) { }

			const Action _action;
			const States::ID _state_id;
		};

	private:
		std::vector<State::ptr> _stack;
		std::vector<PendingChange> _pending_list;

		State::Context _context;
		std::unordered_map<States::ID, std::function<State::ptr()>> _factories;

		bool _is_paused{false};
	};

	template<typename T, typename... Args>
	void StateStack::register_state(const States::ID& state_id, const Args&&... args)
	{
		_factories[state_id] = [this]()
		{
			return State::ptr(new T(*this, _context, args));
		};
	}
}