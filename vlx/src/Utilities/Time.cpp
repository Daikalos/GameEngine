#include <Velox/Utilities/Time.h>

using namespace vlx;

constexpr float Time::GetDT() const noexcept					{ return m_delta_time * m_scaled_time; }
constexpr float Time::GetRealDT() const noexcept				{ return m_delta_time; }
constexpr float Time::GetFixedDT() const noexcept				{ return m_fixed_delta_time * m_scaled_time; }
constexpr float Time::GetRealFixedDT() const noexcept			{ return m_fixed_delta_time; }
constexpr float Time::GetScaledTime() const noexcept			{ return m_scaled_time; }
constexpr float Time::GetInterp() const noexcept				{ return m_interp; }
constexpr long double Time::GetTotalTime() const noexcept		{ return m_total_time; }
constexpr long double Time::GetTotalRunTime() const noexcept	{ return m_total_run_time; }
constexpr int Time::GetFramerate() const noexcept				{ return (int)(1.0f / GetRealDT()); }

void Time::SetScaledTime(const float value) noexcept
{ 
	m_scaled_time = value; 
}

void Time::SetInterp(const float value) noexcept
{
	m_interp = value;
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

	m_total_time += GetRealDT();
	m_total_run_time += GetDT();
}