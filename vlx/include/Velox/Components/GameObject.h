#pragma once

#include <Velox/ECS/IComponent.h>

namespace vlx
{
	/// <summary>
	/// Used for common properties in objects
	/// </summary>
	struct GameObject : public IComponent
	{
		bool is_alive{true};
		bool is_static{true};
	};
}