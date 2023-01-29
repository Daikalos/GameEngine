#pragma once

#include <Velox/Utilities/NonCopyable.h>

#include "Identifiers.hpp"

namespace vlx
{
	class EntityAdmin;

	/// <summary>
	///		Interface for component references
	/// </summary>
	class IComponentRef : private NonCopyable
	{
	public:
		IComponentRef(const EntityID entity_id)
			: m_entity_id(entity_id) { };

		virtual ~IComponentRef() = default;

	public:
		[[nodiscard]] constexpr EntityID GetEntityID() const noexcept	{ return m_entity_id; }

	private:
		virtual void Update(const EntityAdmin&, void*) = 0;

	protected:
		EntityID m_entity_id {NULL_ENTITY};

		friend class EntityAdmin;
	};
}