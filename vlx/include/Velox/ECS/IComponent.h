#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/Utilities/Concepts.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class EntityAdmin;

	/// <summary>
	/// Interface for common component events such as when the component has been created, moved, or destroyed.
	/// </summary>
	class VELOX_API IComponent
	{
	public:
		template<IsComponent>
		friend struct ComponentAlloc;

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
		/// Called when the component has been destroyed, which can include when the entity is removed
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was removed</param>
		virtual void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) {}
	};

	inline IComponent::~IComponent() = default;
}