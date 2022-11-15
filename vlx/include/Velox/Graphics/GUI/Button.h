#pragma once

#include <SFML/Graphics.hpp>

#include <functional>

#include <Velox/ECS/IComponent.h>
#include <Velox/Utilities.hpp>

namespace vlx::gui
{
	/// <summary>
	///		The button only contains events for certain behaviours such as, press, release and hover.
	/// </summary>
	class Button : public IComponent
	{
		vlx::Event<> press;
		vlx::Event<> release;
		vlx::Event<> hover;
	};
}