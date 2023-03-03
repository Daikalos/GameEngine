#pragma once

#include <Velox/ECS/IComponent.h>

namespace vlx
{
	/// Denotes common properties for rendering
	///
	struct Renderable : public IComponent
	{
		bool IsActive	{true};
		bool IsStatic	{false};
		bool IsGUI		{false};
		bool IsVisible	{true};
	};
}