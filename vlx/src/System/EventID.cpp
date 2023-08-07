#include <Velox/System/EventID.h>

#include <cassert>
#include <utility>

using namespace vlx;

EventID::EventID(IEvent& event, evnt::IDType id)
	: m_event(&event), m_id(id)
{

}

EventID::~EventID()
{
	if (IsConnected())
		m_event->RemoveID(m_id);
}

EventID::EventID(EventID&& rhs) noexcept
	: m_event(rhs.m_event), m_id(rhs.m_id)
{
	rhs.m_event = nullptr;
	rhs.m_id = NULL;
}

EventID& EventID::operator=(EventID&& rhs) noexcept
{
	std::swap(m_event, rhs.m_event);
	std::swap(m_id, rhs.m_id);

	return *this;
}

bool EventID::IsConnected() const noexcept
{
	return m_event != nullptr && m_id != NULL;
}

bool EventID::Connect(IEvent& event, evnt::IDType id)
{
	if (IsConnected())
		return false;
	
	m_event = &event;
	m_id = id;

	return true;
}

bool EventID::Disconnect()
{
	if (!IsConnected())
		return false;

	bool result = m_event->RemoveID(m_id); // event might already have been manually removed

	m_event = nullptr; // invalidate
	m_id = NULL;

	return result;
}
