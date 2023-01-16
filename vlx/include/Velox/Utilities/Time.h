#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <Velox/Config.hpp>

#include "NonCopyable.h"

namespace vlx
{
	static constexpr float DEFAULT_FIXED_DELTATIME = 1.0f / 60.0f;
	static constexpr float DEFAULT_SCALED_TIME = 1.0f;

	class VELOX_API Time final : public NonCopyable
	{
	public:
		[[nodiscard]] constexpr float GetDT() const noexcept;
		[[nodiscard]] constexpr float GetRealDT() const noexcept;
		[[nodiscard]] constexpr float GetFixedDT() const noexcept;
		[[nodiscard]] constexpr float GetRealFixedDT() const noexcept;
		[[nodiscard]] constexpr float GetScaledTime() const noexcept;
		[[nodiscard]] constexpr float GetInterp() const noexcept;
		[[nodiscard]] constexpr long double GetTotalTime() const noexcept;
		[[nodiscard]] constexpr long double GetTotalRunTime() const noexcept;
		[[nodiscard]] constexpr int GetFramerate() const noexcept;

		void SetScaledTime(const float value) noexcept;
		void SetInterp(const float value) noexcept;

	public:
		void Reset();

		/// <summary>
		///		Update to receive the latest dt, put first in loop
		/// </summary>
		void Update();

	private:
		float		m_delta_time		{0.0f};						// total time it took for previous loop
		float		m_fixed_delta_time	{DEFAULT_FIXED_DELTATIME};	// fixed delta time for physics etc. (Is set to 1/60 as default)
		float		m_scaled_time		{DEFAULT_SCALED_TIME};		// scaled time (set to 1 as default)
		float		m_interp			{0.0f};

		long double	m_total_time		{0.0};	// total time in seconds the applicaton has ran
		long double	m_total_run_time	{0.0};	// total time the application has ran factoring in scaled time

		sf::Clock	m_clock;
	};
}