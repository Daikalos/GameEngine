#pragma once

#include <Velox/Config.hpp>

namespace vlx
{
	struct VELOX_API ISystemObject
	{
		virtual ~ISystemObject() = 0;
		virtual void Update() = 0;
	};

	inline ISystemObject::~ISystemObject() = default;
}