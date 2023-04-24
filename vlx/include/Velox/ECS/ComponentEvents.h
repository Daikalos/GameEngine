#pragma once

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	class EntityAdmin;

	/// Downside to using interface is that inherited classes will contain a vtable pointer that occupies 8 bytes. However, the interface is almost 
	/// a must for certain concepts to function correctly, for example, scene graph, if a parent entity is suddenly destroyed, the child entities 
	/// must have their data updated to reflect the new state. There are other way to deal with this scenario, for example, a dedicated system. 
	/// Because, after all, components is supposed to only contain data to adhere to the ECS paradigm. But I believe that this idea can be restrictive 
	/// and prone to errors. For example, updating the position of a transform will require the matrix to be rebuilt, and if constructed like a POD, makes 
	/// it difficult for this to be recognized. For each of these cases, a system will have to be built that notices these changes and I feel this can quickly get
	/// out of hand. That is why I favor balancing the data-oriented design with the object-oriented design to maintain readability, maintainability, and usability. 
	/// I believe that performance will not be impacted too much either from this choice. Otherwise, CRTP or a close alternative is being considered.

	///	Called when the component is first created.
	/// 
	/// \param EntityAdmin: The owning admin
	/// \param EntityID: The entity of which this component was added
	/// 
	template<class C>
	class CreatedEvent
	{
		void Created(const EntityAdmin& entity_admin, const EntityID entity_id)
		{
			static_cast<C*>(this)->CreatedImpl(entity_admin, entity_id);
		}

		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};

	///	Called before the component is destroyed, which can occur when the entity or the component alone is removed.
	/// 
	/// \param EntityAdmin: The owning admin
	/// \param EntityID: The entity on which this component was removed
	/// 
	template<class C>
	class DestroyedEvent
	{
		void Destroyed(const EntityAdmin& entity_admin, const EntityID entity_id)
		{
			static_cast<C*>(this)->DestroyedImpl(entity_admin, entity_id);
		}

		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};

	///	Called on the new component that was created by copying another component.
	/// 
	/// \param EntityAdmin: The owning admin
	/// \param EntityID: The entity on which this component was copied
	/// 
	template<class C>
	class CopiedEvent
	{
		void Copied(const EntityAdmin& entity_admin, const EntityID entity_id)
		{
			static_cast<C*>(this)->CopiedImpl(entity_admin, entity_id);
		}

		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};

	///	Called right before this component's data is modified.
	/// 
	/// \param EntityAdmin: The owning admin
	/// \param EntityID: The entity on which this component was copied
	/// \param NewData: The new data that will modify the current data
	/// 
	template<class C>
	class AlteredEvent
	{
		void Altered(const EntityAdmin& entity_admin, const EntityID entity_id, C& new_data)
		{
			static_cast<C*>(this)->AlteredImpl(entity_admin, entity_id, new_data);
		}

		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};

	///	Called on the new component that was created by moving the data from another component.
	/// 
	/// \param EntityAdmin: The owning admin
	/// \param EntityID: The entity on which this component was moved
	/// 
	template<class C>
	class MovedEvent
	{
		void Moved(const EntityAdmin& entity_admin, const EntityID entity_id)
		{
			static_cast<C*>(this)->MovedImpl(entity_admin, entity_id);
		}

		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};

	///	Called before the EntityAdmin is destroyed.
	/// 
	/// \param EntityAdmin: The owning admin
	/// \param EntityID: The entity on which this component was removed
	/// 
	template<class C>
	class ShutdownEvent
	{
		void Shutdown(const EntityAdmin& entity_admin, const EntityID entity_id)
		{
			static_cast<C*>(this)->ShutdownImpl(entity_admin, entity_id);
		}

		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};
}