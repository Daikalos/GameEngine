#pragma once

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API NonCopyable // custom NonCopyable because sfml's gives weird linking error
	{
	protected:
		NonCopyable() = default;
		~NonCopyable() = default;

	private:
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator =(const NonCopyable&) = delete;
	};
}