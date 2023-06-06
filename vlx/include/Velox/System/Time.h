#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	inline constexpr float DEFAULT_FIXED_DELTATIME = 1.0f / 60.0f;
	inline constexpr float DEFAULT_SCALED_TIME = 1.0f;

	class VELOX_API Time final
	{
	public:
		NODISC float GetDT() const noexcept;
		NODISC float GetRealDT() const noexcept;
		NODISC float GetFixedDT() const noexcept;
		NODISC float GetRealFixedDT() const noexcept;
		NODISC float GetScaledTime() const noexcept;
		NODISC float GetAlpha() const noexcept;
		NODISC long double GetTotalTime() const noexcept;
		NODISC long double GetTotalRunTime() const noexcept;
		NODISC int GetFPS() const noexcept;
		NODISC int GetFixedFPS() const noexcept;

		void SetScaledTime(const float value) noexcept;
		void SetAlpha(const float value) noexcept;

	public:
		void Reset();

		///	Update to receive the latest dt, put first in loop
		/// 
		void Update();

	private:
		sf::Clock	m_clock;

		float		m_delta_time		{0.0f};						// total time it took for previous loop
		float		m_fixed_delta_time	{DEFAULT_FIXED_DELTATIME};	// fixed delta time for physics etc. (Is set to 1/60 as default)
		float		m_scaled_time		{DEFAULT_SCALED_TIME};		// scaled time (set to 1 as default)
		float		m_alpha				{0.0f};

		long double	m_total_time		{0.0};	// total time in seconds the applicaton has ran
		long double	m_total_run_time	{0.0};	// total time the application has ran factoring in scaled time
	};
}