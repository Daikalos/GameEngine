#pragma once

#include <string>
#include <string_view>

#include <Velox/Config.hpp>

namespace vlx::su
{
	VELOX_API NODISC std::string RemoveTrailingZeroes(std::string_view str);
}
