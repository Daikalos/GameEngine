#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

#include <SFML/Window/Event.hpp>

#include <Velox/Utility/NonCopyable.h>

#include <Velox/Types.hpp>
#include <Velox/Config.hpp>

#include "State.h"
#include "Identifiers.h"

namespace vlx
{
	class World;

	/// Holds all the active states in a stack that can be easily modified.
	/// 
	class VELOX_API StateStack : private NonCopyable
	{
	private:
		using StatePtr = typename State::Ptr;
		using StateFunc = typename State::Func;

		using Stack		= std::vector<StatePtr>;
		using Factory	= std::unordered_map<StateID, StateFunc>;

		enum class Action : uint8
		{
			Push,
			Pop,
			Erase,
			Clear
		};

		struct PendingChange
		{
			explicit PendingChange(const Action& action, StateID state_id = -1);

			Action	action;
			StateID	state_id;
		};

		using PendingList = std::vector<PendingChange>;

	public:
		StateStack(World& world);

	public:
		NODISC bool IsEmpty() const noexcept;
		NODISC bool IsPaused() const noexcept;

		void SetPaused(bool flag);

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

	public:
		template<std::derived_from<State> T, typename... Args>
			requires std::constructible_from<T, StateID, StateStack&, World&, Args...>
		void RegisterState(StateID state_id, Args&&... args);

	private:
		auto CreateState(StateID state_id) -> StatePtr;
		void ApplyPendingChanges();

	private:
		World*		m_world	{nullptr};

		Stack		m_stack;		// all states are stored here
		Factory		m_factory;		// factory for storing functions that creates the registered object
		PendingList	m_pending_list; // pending list of changes to the stack, applied in post update

		bool		m_paused {false};

		friend class State;
	};

	template<std::derived_from<State> T, typename... Args>
		requires std::constructible_from<T, StateID, StateStack&, World&, Args...>
	void StateStack::RegisterState(StateID state_id, Args&&... args)
	{
		m_factory[state_id] = [this, state_id, ... args = std::forward<Args>(args)]()
		{
			return std::make_unique<T>(state_id, *this, *m_world, args...);
		};
	}
}