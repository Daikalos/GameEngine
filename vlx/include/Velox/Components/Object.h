#pragma once

#include <Velox/ECS/IComponent.h>

namespace vlx
{
	/// <summary>
	///		Used for common properties in objects, usually all entities has this component but does not need to
	/// </summary>
	struct Object : public IComponent
	{
		bool is_alive	{true};
		bool is_static	{false};
		bool is_active	{true};
	};
}