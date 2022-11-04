#pragma once

#include <Velox/ECS/IComponent.h>

namespace vlx
{
	/// <summary>
	/// Used for common properties in objects, usually everything makes use of this component
	/// </summary>
	struct Object : public IComponent
	{
		bool is_alive	{true};
		bool is_static	{false};
	};
}