#pragma once

#include <Velox/Utilities/Concepts.h>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	class EntityAdmin;

	/// <summary>
	/// Interface for common component events such as when the component has been created, moved, or destroyed.
	/// </summary>
	class VELOX_API IComponent
	{
	public:
		virtual ~IComponent() = 0; // to make the interface abstract

	protected:
		/// <summary>
		/// Called when the component is first created
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was added</param>
		virtual void Created(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		/// <summary>
		/// Called when the component has been moved to another location in memory
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was moved</param>
		virtual void Moved(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		/// <summary>
		/// Called when the component has been copied to another location in memory
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was copied</param>
		virtual void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		/// <summary>
		/// Called before the component is destroyed, which can occur when the entity or component is removed
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was removed</param>
		virtual void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) {}

	private:
		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};

	inline IComponent::~IComponent() = default;
}