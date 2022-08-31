#include <Velox/Utilities/Time.h>

using namespace vlx;

[[nodiscard]] constexpr float Time::GetDeltaTime() const noexcept			{ return m_delta_time * m_scaled_time; }
[[nodiscard]] constexpr float Time::GetRealDeltaTime() const noexcept		{ return m_delta_time; }
[[nodiscard]] constexpr float Time::GetFixedDeltaTime() const noexcept		{ return m_fixed_delta_time * m_scaled_time; }
[[nodiscard]] constexpr float Time::GetRealFixedDeltaTime() const noexcept	{ return m_fixed_delta_time; }
[[nodiscard]] constexpr float Time::GetScaledTime() const noexcept			{ return m_scaled_time; }
[[nodiscard]] constexpr long double Time::GetTotalTime() const noexcept		{ return m_total_time; }
[[nodiscard]] constexpr long double Time::GetTotalRunTime() const noexcept	{ return m_total_run_time; }
[[nodiscard]] constexpr int Time::GetFramerate() const noexcept				{ return 1.0f / GetRealDeltaTime(); }

void Time::SetScaledTime(const float value) noexcept
{ 
	m_scaled_time = value; 
}

void Time::Reset()
{
	m_clock.restart();
	m_fixed_delta_time = DEFAULT_FIXED_DELTATIME;
	m_scaled_time = DEFAULT_SCALED_TIME;
}

void Time::Update()
{
	m_delta_time = m_clock.restart().asSeconds();

	m_total_time += GetRealDeltaTime();
	m_total_run_time += GetDeltaTime();
}