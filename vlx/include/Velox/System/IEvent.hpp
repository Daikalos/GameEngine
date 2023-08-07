#pragma once

#include "EventIdentifiers.h"

namespace vlx
{
	class IEvent
	{
	public:
		virtual ~IEvent() = default;

		virtual evnt::IDType operator-=(evnt::IDType handler_id) = 0;
		virtual bool RemoveID(evnt::IDType handler_id) = 0;

		virtual std::size_t Count() const noexcept = 0;
		virtual bool IsEmpty() const noexcept = 0;

		virtual void Reserve(std::size_t size) = 0;
		virtual void Clear() noexcept = 0;
	};
}