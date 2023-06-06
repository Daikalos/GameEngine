#pragma once

#include <string>

namespace vlx
{
	///	Used for common properties in objects, essential for many systems. 
	/// 
	struct Object
	{
		std::string tag;
		bool activeHierarchy	{true};
		bool activeSelf			{true};
	};

	constexpr int a = sizeof(Object);
}