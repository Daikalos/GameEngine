#include <Velox/ECS/SystemBase.h>

using namespace vlx;

float SystemBase::GetPriority() const noexcept		{ return m_priority; }
bool SystemBase::IsRunningParallel() const noexcept { return m_run_parallel; }
bool SystemBase::IsEnabled() const noexcept			{ return m_enabled; }

void SystemBase::SetPriority(float val)		{ m_priority = val; }
void SystemBase::SetRunParallel(bool flag)	{ m_run_parallel = flag; }
void SystemBase::SetEnabled(bool flag)		{ m_enabled = flag; }

void SystemBase::Start() const {}
void SystemBase::End() const {}