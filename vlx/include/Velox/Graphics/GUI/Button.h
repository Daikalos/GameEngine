#pragma once

#include <Velox/Utilities.hpp>

#include "GUIComponent.h"

namespace vlx::gui
{
	/// <summary>
	///		The button contains events for certain behaviours, e.g., press, release, and hover.
	///		They are usually called when the GUI component is given the respective inputs, but can be manually
	///		called if the developer wants to.
	/// </summary>
	struct VELOX_API Button final : public GUIComponent
	{
		using GUIComponent::GUIComponent; // inherit constructor

		vlx::Event<> Pressed;
		vlx::Event<> Released;
		vlx::Event<> Hovered;

		[[nodiscard]] constexpr bool IsSelectable() const noexcept override;
	};
}