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
int			Time::GetFPS() const noexcept			{ return (int)std::round(1.0f / GetRealDT()); }
int			Time::GetFixedFPS() const noexcept		{ return (int)std::round(1.0f / GetRealFixedDT()); }

void Time::SetScaledTime(float value) noexcept
{ 
	m_scaled_time = value; 
}

void Time::SetAlpha(float value) noexcept
{
	m_alpha = value;
}

void Time::Reset()
{
	m_clock.restart();
	m_fixed_delta_time	= T_DEFAULT_FIXED_DELTATIME;
	m_scaled_time		= T_DEFAULT_SCALED_TIME;
}

void Time::Update()
{
	float dt = m_clock.restart().asSeconds();
	m_delta_time = (dt > T_MAX_DELTATIME) ? T_MAX_DELTATIME : dt;

	m_total_time		+= GetRealDT();
	m_total_run_time	+= GetDT();
}