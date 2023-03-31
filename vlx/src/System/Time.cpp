#include <Velox/System/Time.h>

using namespace vlx;

float		Time::GetDT() const noexcept			{ return m_delta_time * m_scaled_time; }
float		Time::GetRealDT() const noexcept		{ return m_delta_time; }
float		Time::GetFixedDT() const noexcept		{ return m_fixed_delta_time * m_scaled_time; }
float		Time::GetRealFixedDT() const noexcept	{ return m_fixed_delta_time; }
float		Time::GetScaledTime() const noexcept	{ return m_scaled_time; }
float		Time::GetAlpha() const noexcept			{ return m_alpha; }
long double	Time::GetTotalTime() const noexcept		{ return m_total_time; }
long double	Time::GetTotalRunTime() const noexcept	{ return m_total_run_time; }
int			Time::GetFPS() const noexcept			{ return std::round(1.0f / GetRealDT()); }
int			Time::GetFixedFPS() const noexcept		{ return std::round(1.0f / GetRealFixedDT()); }

void Time::SetScaledTime(const float value) noexcept
{ 
	m_scaled_time = value; 
}

void Time::SetAlpha(const float value) noexcept
{
	m_alpha = value;
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