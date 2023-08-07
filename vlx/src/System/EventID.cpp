#include <Velox/System/EventID.h>

#include <cassert>

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

EventID::EventID(EventID&& event_id)
	: m_event(event_id.m_event), m_id(event_id.m_id)
{
	event_id.m_event = nullptr;
	event_id.m_id = NULL;
}

EventID& EventID::operator=(EventID&& event_id)
{
	if (IsConnected())
		m_event->RemoveID(m_id);

	m_event = event_id.m_event;
	m_id = event_id.m_id;

	event_id.m_event = nullptr;
	event_id.m_id = NULL;

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
