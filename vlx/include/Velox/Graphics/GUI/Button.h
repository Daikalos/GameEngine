#pragma once

#include <SFML/Graphics.hpp>

#include <functional>

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities.hpp>

namespace vlx::gui
{
	/// <summary>
	///		The button contains events for certain behaviours, e.g., press, release, and hover.
	///		They are usually called when the GUI component is given the respective inputs, but can be manually
	///		called if the developer wants to
	/// </summary>
	struct Button final : public IComponent
	{
		vlx::Event<> Pressed;
		vlx::Event<> Released;
		vlx::Event<> Hovered;
	};
}