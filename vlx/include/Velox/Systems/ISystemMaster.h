#pragma once

#include <Velox/Config.hpp>

namespace vlx
{
	struct VELOX_API ISystemMaster
	{
		virtual ~ISystemMaster() = 0;

		virtual void Update() = 0;

	private:
		virtual void PreUpdate() {}
		virtual void PostUpdate() {}
	};

	inline ISystemMaster::~ISystemMaster() = default;
}