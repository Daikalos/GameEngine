#pragma once

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	class EntityAdmin;

	///	Called when the component is first created.
	/// 
	/// \param EntityAdmin: The owning admin
	/// \param EntityID: The entity of which this component was added
	/// 
	template<class C>
	class CreatedEvent
	{
		void Created(const EntityAdmin& entity_admin, EntityID entity_id)
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
		void Destroyed(const EntityAdmin& entity_admin, EntityID entity_id)
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
		void Copied(const EntityAdmin& entity_admin, EntityID entity_id)
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
		void Altered(const EntityAdmin& entity_admin, EntityID entity_id, C& new_data)
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
		void Moved(const EntityAdmin& entity_admin, EntityID entity_id)
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
		void Shutdown(const EntityAdmin& entity_admin, EntityID entity_id)
		{
			static_cast<C*>(this)->ShutdownImpl(entity_admin, entity_id);
		}

		template<IsComponent>
		friend struct ComponentAlloc;

		friend class EntityAdmin;
	};

	template<class C, template<class> class... Events>
	struct EventSet : public Events<C>... {};

	template<class C, template<class> class E>
	concept HasEvent = requires(C c)
	{
		[]<typename T>(E<T>&){}(c);
	};
}