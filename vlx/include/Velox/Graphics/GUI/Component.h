#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/Utilities.hpp>
#include <Velox/ECS/IComponent.h>
#include <Velox/Components/Relation.hpp>

namespace vlx::gui
{
	class Component : public Relation<Component>
	{
	public:
		const bool& IsActive() const noexcept;

	public:
		void Activate();
		void Deactivate();

	private:
		bool m_active	{false};
	};
}

