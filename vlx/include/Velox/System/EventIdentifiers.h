#pragma once

#include <atomic>

#include <Velox/Types.hpp>

namespace vlx
{
	namespace evnt
	{
		using IDType	= uint32;
		using AtomicID	= std::atomic<IDType>;
	}
}