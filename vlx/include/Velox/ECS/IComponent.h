#pragma once

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	class EntityAdmin;

	///	Interface for components and contain events such as when it has been created, moved, or destroyed.
	/// Downside to using interface is that inherited classes will contain a vtable pointer that occupies 8 bytes.
	/// However, the interface is almost a must for certain concepts to function correctly, for example, scene graph.
	///
	class VELOX_API IComponent
	{
	public:
		virtual ~IComponent() = 0; // to make the interface abstract

	protected:
		///	Called when the component is first created.
		/// 
		/// \param EntityAdmin: The owning admin.
		/// \param EntityID: The entity of which this component was added.
		/// 
		virtual void Created(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		///	Called on the new component that was created by moving the data from another component.
		/// 
		/// \param EntityAdmin: The owning admin.
		/// \param EntityID: The entity on which this component was moved.
		/// 
		virtual void Moved(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		///	Called on the new component that was created by copying another component.
		/// 
		/// \param EntityAdmin: The owning admin.
		/// \param EntityID: The entity on which this component was copied.
		/// 
		virtual void Copied(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		///	Called right before the component's data has been modified.
		/// 
		/// \param EntityAdmin: The owning admin.
		/// \param EntityID: The entity on which this component was copied.
		/// \param NewData: The new data that will modify the current data.
		/// 
		virtual void Modified(const EntityAdmin& entity_admin, const EntityID entity_id, IComponent& new_data) {}

		///	Called before the component is destroyed, which can occur when the entity or component is removed.
		/// 
		/// \param EntityAdmin: The owning admin.
		/// \param EntityID: The entity on which this component was removed.
		/// 
		virtual void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id) {}

		///	Called before the EntityAdmin is destroyed.
		/// 
		/// \param EntityAdmin: The owning admin
		/// \param EntityID: The entity on which this component was removed
		/// 
		virtual void Shutdown(const EntityAdmin& entity_admin, const EntityID entity_id) {}

	private:
		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};

	inline IComponent::~IComponent() = default;
}