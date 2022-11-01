#pragma once

#include <Velox/Config.hpp>

namespace vlx
{
	struct VELOX_API ISystemObject
	{
		virtual ~ISystemObject() = default;
		virtual void Update() = 0;
	};
}