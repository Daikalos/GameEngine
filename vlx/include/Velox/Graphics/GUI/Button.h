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
	struct Button final : public GUIComponent
	{
		vlx::Event<> Pressed;
		vlx::Event<> Released;
		vlx::Event<> Hovered;
	};
}