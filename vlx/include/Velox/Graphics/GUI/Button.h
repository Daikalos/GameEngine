#pragma once

#include <Velox/Utilities.hpp>

#include "GUIComponent.h"

namespace vlx::gui
{
	/// <summary>
	///		The button contains events for certain behaviours, e.g., press, release, and hover. They are usually 
	///		called when the GUI component is given the respective inputs, but can be manually called if the developer wants to.
	/// </summary>
	class VELOX_API Button final : public GUIComponent
	{
	public:
		using GUIComponent::GUIComponent; // inherit constructor

	public:
		[[nodiscard]] constexpr bool IsSelectable() const noexcept override;

	public:
		void Click();
		void Press();
		void Release();
		void Enter();
		void Exit();

	public:
		vlx::Event<> Clicked;
		vlx::Event<> Pressed;
		vlx::Event<> Released;
		vlx::Event<> Entered;
		vlx::Event<> Exited;

	private:
		bool m_pressed{false};
		bool m_entered{false};
	};
}