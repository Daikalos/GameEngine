#pragma once

#include <string>
#include <string_view>

namespace vlx
{
	namespace su
	{
		static constexpr std::string RemoveTrailingZeroes(std::string_view str)
		{
			std::string result = std::string(str.data());

			if (result.find('.') != std::string::npos)
			{
				result = result.substr(0, result.find_last_not_of('0') + 1);

				if (result.find('.') == result.size() - 1)
					result = result.substr(0, result.size() - 1);
			}

			return result;
		}
	}
}
