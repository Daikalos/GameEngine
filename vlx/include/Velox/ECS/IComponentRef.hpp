#pragma once

#include <Velox/Utilities.hpp>

namespace vlx
{
	class IComponentRef : private NonCopyable
	{
	public:
		virtual ~IComponentRef() = default;

		virtual constexpr bool IsValid() const noexcept = 0;
		virtual constexpr bool IsExpired() const = 0;

		virtual void Reset() = 0;
		virtual void ForceUpdate() = 0;
	};
}