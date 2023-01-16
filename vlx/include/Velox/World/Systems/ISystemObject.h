#pragma once

#include <Velox/ECS/Identifiers.hpp>

#include <Velox/Config.hpp>
#include <memory>

namespace vlx
{
	class VELOX_API ISystemObject
	{
	public:
		using Ptr = std::shared_ptr<ISystemObject>;

	public:
		virtual ~ISystemObject() = default;

	public:
		virtual constexpr LayerType GetID() const noexcept = 0;

	public:
		virtual void Update() = 0;
	};
}