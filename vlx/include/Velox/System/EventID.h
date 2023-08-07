#pragma once

#include "IEvent.hpp"

#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API EventID
	{
	public:
		EventID() = default;

		explicit EventID(IEvent& event, evnt::IDType id);
		~EventID();

		EventID(EventID&& event_id);
		EventID(const EventID& event_id) = delete;

		EventID& operator=(EventID&& event_id);
		EventID& operator=(const EventID& event_id) = delete;

	public:
		bool IsConnected() const noexcept;

		bool Connect(IEvent& event, evnt::IDType id);
		bool Disconnect();

	private:
		IEvent*			m_event	{nullptr};
		evnt::IDType	m_id	{0};
	};
}