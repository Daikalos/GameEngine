#pragma once

#include <SFML/System/Clock.hpp>

#include "NonCopyable.h"

namespace vlx
{
	static const float DEFAULT_FIXED_DELTATIME = 1.0f / 60.0f;
	static const float DEFAULT_SCALED_TIME = 1.0f;

	class Time final : public NonCopyable
	{
	public:
		Time() : 
			m_delta_time(float()), m_fixed_delta_time(DEFAULT_FIXED_DELTATIME), m_scaled_time(DEFAULT_SCALED_TIME), 
			m_total_time(float()), m_total_run_time(float()) { }

		[[nodiscard]] constexpr float GetDeltaTime() const						{ return m_delta_time * m_scaled_time; }
		[[nodiscard]] constexpr float GetRealDeltaTime() const noexcept			{ return m_delta_time; }
		[[nodiscard]] constexpr float GetFixedDeltaTime() const noexcept		{ return m_fixed_delta_time * m_scaled_time; }
		[[nodiscard]] constexpr float GetRealFixedDeltaTime() const noexcept	{ return m_fixed_delta_time; }
		[[nodiscard]] constexpr float GetScaledTime() const noexcept			{ return m_scaled_time; }
		[[nodiscard]] constexpr long double GetTotalTime() const noexcept		{ return m_total_time; }
		[[nodiscard]] constexpr long double GetTotalRunTime() const noexcept	{ return m_total_run_time; }
		[[nodiscard]] constexpr int GetFramerate() const noexcept				{ return 1.0f / GetRealDeltaTime(); }

		void SetScaledTime(const float value)									{ m_scaled_time = value; }

		void Reset()
		{
			m_clock.restart();
			m_fixed_delta_time = DEFAULT_FIXED_DELTATIME;
			m_scaled_time = DEFAULT_SCALED_TIME;
		}

		////////////////////////////////////////////////////////
		// Update to receive the latest dt, put first in loop
		////////////////////////////////////////////////////////
		void Update()
		{
			m_delta_time = m_clock.restart().asSeconds();

			m_total_time += GetRealDeltaTime();
			m_total_run_time += GetDeltaTime();
		}

	private:
		float		m_delta_time;		// total time it took for previous loop
		float		m_fixed_delta_time;	// fixed delta time for physics etc. (Is set to 1/60 as default)
		float		m_scaled_time;		// scaled time (set to 1 as default)

		long double	m_total_time;		// total time in seconds the applicaton has ran
		long double	m_total_run_time;	// total time the application has ran factoring in scaled time

		sf::Clock	m_clock;
	};
}