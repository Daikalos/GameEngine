#pragma once

#include <array>

#include <Velox/ECS/Identifiers.hpp>
#include <Velox/System/Vector2.hpp>
#include <Velox/VeloxTypes.hpp>

namespace vlx
{
	class CollisionResult
	{
	private:
		struct Contact
		{
			Vector2f		normal;
			Vector2f		hit;
			float			penetration {0.0f};
		};

	private:
		using Contacts = std::array<Contact, 2>;

	public:
		EntityID	other			{NULL_ENTITY};
		Contacts	contacts;
		uint8		contacts_count	{0};
	};
}