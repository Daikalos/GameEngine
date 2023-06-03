#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

#include <Velox/Config.hpp>
#include <Velox/Utility/NonCopyable.h>

#include "State.h"
#include "Identifiers.h"

namespace vlx
{
	/// Holds all the active states in a stack that can be easily modified.
	/// 
	class VELOX_API StateStack : private NonCopyable
	{
	private:
		using Stack		= std::vector<typename State::Ptr>;
		using Factory	= std::unordered_map<StateID, typename State::Func>;

		enum class Action
		{
			Push,
			Pop,
			Erase,
			Clear
		};

		struct PendingChange
		{
			explicit PendingChange(const Action& action, StateID state_id = -1);

			const Action	action;
			const StateID	state_id;
		};

	public:
		StateStack(World& world);

	public:
		NODISC bool IsEmpty() const noexcept;
		NODISC bool IsPaused() const noexcept;

		void SetPaused(bool flag);

	public:
		template<std::derived_from<State> T, typename... Args>
		void RegisterState(StateID state_id, Args&&... args);

	public:
		void HandleEvent(const sf::Event& event);

		void Start(Time& time);
		void PreUpdate(Time& time);
		void Update(Time& time);
		void FixedUpdate(Time& time);
		void PostUpdate(Time& time);

		void Draw();

		void Push(StateID state_id);
		void Pop();
		void Clear();

	private:
		auto CreateState(StateID state_id) -> typename State::Ptr;
		void ApplyPendingChanges();

	private:
		World*						m_world	{nullptr};

		Stack						m_stack;		// all states are stored here
		Factory						m_factory;		// factory for storing functions that creates the registered object
		std::vector<PendingChange>	m_pending_list; // pending list of changes to the stack, applied in post update

		bool m_paused {false};

		friend class State;
	};

	template<std::derived_from<State> T, typename... Args>
	void StateStack::RegisterState(StateID state_id, Args&&... args)
	{
		m_factory[state_id] = [this, &state_id, &args...]()
		{
			return typename State::Ptr(new T(*this, *m_world, state_id, std::forward<Args>(args)...));
		};
	}
}