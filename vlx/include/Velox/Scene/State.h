#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <functional>

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
		/// <summary>
		///		OnCreated is called when first constructed
		/// </summary>
		virtual void OnCreated() {}

		/// <summary>
		///		OnActivate is called whenever the state is put as last in the stack
		/// </summary>
		virtual void OnActivate() {}

		/// <summary>
		///		OnDestroy is called when the state is removed from the stack
		/// </summary>
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