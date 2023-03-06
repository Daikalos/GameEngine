#pragma once

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	class EntityAdmin;

	/// <summary>
	///		Interface for components and contain events such as when it has been created, moved, or destroyed.
	/// </summary>
	class VELOX_API IComponent
	{
	public:
		virtual ~IComponent() = 0; // to make the interface abstract

	protected:
		/// <summary>
		///		Called when the component is first created
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was added</param>
		virtual void Created(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		/// <summary>
		///		Called on the new component that was created by moving the data from another component
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was moved</param>
		virtual void Moved(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		/// <summary>
		///		Called on the new component that was created by copying another component
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was copied</param>
		virtual void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		/// <summary>
		///		Called right before the component's data has been modified
		/// </summary>
		/// <param name="entity_admin:">The master entity admin</param>
		/// <param name="entity_id:">The entity on which this component was copied</param>
		/// <param name="new_component:">The new component that will modify the current data</param>
		virtual void Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data) {}

		/// <summary>
		///		Called before the component is destroyed, which can occur when the entity or component is removed
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