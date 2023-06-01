#include <Velox/Utility/FPSCounter.h>

using namespace vlx;

float FPSCounter::GetFPS() const
{
	return m_ready ? (1.0f / (m_average_fps / BUFFER_SIZE)) : 0.0f;
}

void FPSCounter::Update(const Time& time)
{
	m_average_fps -= m_buffer[m_current];
	m_buffer[m_current] = time.GetRealDT();
	m_average_fps += m_buffer[m_current];

	m_current = (m_current + 1) % BUFFER_SIZE;

	if (m_current == BUFFER_SIZE - 1)
		m_ready = true;
}
