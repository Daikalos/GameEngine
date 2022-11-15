#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/Components/Relation.hpp>

namespace vlx::gui
{
	class Component : public Relation<Component>
	{
	private:
		using SizeType = std::uint16_t;

	public:
		constexpr bool IsActive() const noexcept;

	public:
		void Activate(const EntityAdmin& entity_admin);
		void Deactivate(const EntityAdmin& entity_admin);

		void Select();
		void Unselect();

		void SelectNext();

	public:
		vlx::Event<>	Activated;
		vlx::Event<>	Deactivated;
		vlx::Event<>	Selected;
		vlx::Event<>	Unselected;

	private:
		bool		m_active			{false};
		bool		m_selected			{false};
		SizeType	m_selected_index	{0};
		float		m_opacity			{1.0f};
	};
}

