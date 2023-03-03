#pragma once

#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include <Velox/Config.hpp>
#include <Velox/Utilities.hpp>

#include "Identifiers.h"

namespace vlx
{
	class StateStack;
	class World;

	class VELOX_API State
	{
	public:
		using Ptr = std::unique_ptr<State>;
		using Func = std::function<Ptr()>;

	public:
		State(const StateID state_id, StateStack& state_stack, World& world);
		virtual ~State() = default;

	public:
		[[nodiscard]] constexpr StateID GetID() const noexcept;

	protected:
		[[nodiscard]] const StateStack& GetStack() const;
		[[nodiscard]] StateStack& GetStack();

		[[nodiscard]] const World& GetWorld() const;
		[[nodiscard]] World& GetWorld();

	public:
		///	OnCreated is called when first constructed
		/// 
		virtual void OnCreated() {}

		///	OnActivate is called whenever the state is put as last in the stack
		/// 
		virtual void OnActivate() {}

		///	OnDestroy is called when the state is removed from the stack
		/// 
		virtual void OnDestroy() {}

		virtual bool HandleEvent(const sf::Event& event) = 0;

		virtual bool PreUpdate(Time& time)		{ return true; }
		virtual bool Update(Time& time) = 0;
		virtual bool FixedUpdate(Time& time)	{ return true; }
		virtual bool PostUpdate(Time& time)		{ return true; }

		virtual void Draw() = 0;

	private:
		StateID		m_id			{-1};
		StateStack* m_state_stack	{nullptr};
		World*		m_world			{nullptr};
	};
}