#pragma once

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

#include "Identifiers.hpp"

namespace vlx
{
	class EntityAdmin;

	/// ComponentAlloc is a helper class for altering data according to a specific type
	/// 
	struct IComponentAlloc
	{
		constexpr virtual ~IComponentAlloc() = default;

		virtual void ConstructData(		const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const = 0;
		virtual void DestroyData(		const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const = 0;
		virtual void MoveData(			const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const = 0;
		virtual void MoveDestroyData(	const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const = 0;
		virtual void CopyData(			const EntityAdmin& entity_admin, EntityID entity_id, DataPtr source, DataPtr destination) const = 0;
		virtual void SwapData(			const EntityAdmin& entity_admin, EntityID entity_id, DataPtr d0, DataPtr d1) const = 0;
		virtual void Shutdown(			const EntityAdmin& entity_admin, EntityID entity_id, DataPtr data) const = 0;

		virtual constexpr std::size_t GetSize() const noexcept = 0;
	};
}