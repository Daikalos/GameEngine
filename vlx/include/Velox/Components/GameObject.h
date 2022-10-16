#pragma once

#include <Velox/ECS/IComponent.h>

namespace vlx
{
	struct GameObject : public IComponent
	{
		bool is_alive{true};
		bool is_static{true};
	};
}