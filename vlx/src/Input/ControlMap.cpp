#include <Velox/Input/ControlMap.h>

using namespace vlx;

void ControlMap::UpdateAll(const Time& time, const bool focus)
{
	for (auto& controls : m_controls)
		controls.second->Update(time, focus);
}
void ControlMap::HandleEventAll(const sf::Event& event)
{
	for (auto& controls : m_controls)
		controls.second->HandleEvent(event);
}