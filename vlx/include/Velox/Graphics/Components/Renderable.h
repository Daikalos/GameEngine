#pragma once

#include <Velox/ECS/IComponent.h>

namespace vlx
{
	struct Renderable : public IComponent
	{
		bool IsActive	{true};
		bool IsStatic	{false};
		bool IsGUI		{false};
		bool IsVisible	{true};
	};
}