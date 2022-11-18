#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/Components/Relation.hpp>

namespace vlx::gui
{
	class Component final : public Relation<Component>
	{
	private:
		using SizeType	= std::uint16_t;
		using Vector2Type = sf::Vector2<SizeType>;

	public:
		Component() = default;
		Component(const Vector2Type& size);
		Component(const Vector2Type& size, bool active);
		Component(const Vector2Type& size, float opacity);
		Component(const Vector2Type& size, bool active, float opacity);

		[[nodiscard]] constexpr bool IsActive() const noexcept;
		[[nodiscard]] constexpr bool IsSelected() const noexcept;

	public:
		void Activate(const EntityAdmin& entity_admin);
		void Deactivate(const EntityAdmin& entity_admin);

		void Select();
		void Unselect();

	private:
		void OnAttach(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<Component>& child);
		void OnDetach(const EntityAdmin& entity_admin, const EntityID entity_id, const EntityID child_id, Relation<Component>& child);

	public:
		vlx::Event<> Activated;
		vlx::Event<> Deactivated;
		vlx::Event<> Selected;
		vlx::Event<> Unselected;

	private:
		Vector2Type m_size;
		bool		m_active			{true};
		bool		m_selected			{false};
		float		m_opacity			{1.0f};

		bool		m_parent_active		{true};
	};
}

