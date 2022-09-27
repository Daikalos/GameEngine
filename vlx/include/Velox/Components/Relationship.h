#pragma once

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/Config.hpp>

namespace vlx
{
	class VELOX_API Relationship
	{
	public:
		void AttachParent(const EntityID parent);
		void DetachParent(const EntityID parent);

	private:
		void AttachChild(const Relationship& child);
		void DetachChild(const Relationship& child);

	public:
		EntityID parent	{NULL_ENTITY};
		EntityID first	{NULL_ENTITY};
		EntityID prev	{NULL_ENTITY};
		EntityID next	{NULL_ENTITY};
	};
}