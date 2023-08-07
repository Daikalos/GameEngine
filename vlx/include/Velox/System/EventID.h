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

		EventID(EventID&& rhs) noexcept;
		EventID(const EventID&) = delete;

		EventID& operator=(EventID&& rhs) noexcept;
		EventID& operator=(const EventID&) = delete;

	public:
		bool IsConnected() const noexcept;

		bool Connect(IEvent& event, evnt::IDType id);
		bool Disconnect();

	private:
		IEvent*			m_event	{nullptr};
		evnt::IDType	m_id	{0};
	};
}