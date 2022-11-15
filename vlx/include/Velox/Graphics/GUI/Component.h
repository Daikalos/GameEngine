#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/Components/Relation.hpp>

namespace vlx::gui
{
	class Component : public Relation<Component>
	{
	public:
		constexpr bool IsActive() const noexcept;

	public:
		void Activate(const EntityAdmin& entity_admin);
		void Deactivate(const EntityAdmin& entity_admin);

		void Select();
		void SelectNext();
		void Unselect();

	private:
		bool			m_active	{false};
		bool			m_selected	{false};
		float			m_opacity	{1.0f};

		vlx::Event<>	m_activated;
		vlx::Event<>	m_deactivated;
		vlx::Event<>	m_selected;
		vlx::Event<>	m_unselected;
	};
}

