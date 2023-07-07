#pragma once

#include <string>
#include <string_view>

#include <Velox/Config.hpp>

namespace vlx::su
{
	NODISC VELOX_API std::string RemoveTrailingZeroes(std::string_view str);
}
