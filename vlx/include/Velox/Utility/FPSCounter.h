#pragma once

#include <Velox/System/Time.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API FPSCounter
	{
	private:
		static constexpr int BUFFER_SIZE = 100;

	public:
		float GetFPS() const;

	public:
		void Update(const Time& time);

	private:
		float	m_buffer[BUFFER_SIZE];
		float	m_average_fps	{0.0f};
		int		m_current		{0};
		bool	m_ready			{false};
	};
}