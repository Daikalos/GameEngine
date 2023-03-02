#pragma once

#include <Velox/ECS/IComponent.h>

namespace vlx
{
	///	Used for common properties in objects, essential for many systems. Maybe in the future 
	/// this will be divided into different components to separate concerns.
	/// 
	struct Object : public IComponent
	{
		bool IsAlive	{true};
		bool IsActive	{true};
	};
}