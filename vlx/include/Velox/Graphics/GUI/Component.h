#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>

namespace vlx::gui
{
	class Component final : public IComponent
	{
	private:
		using SizeType		= std::uint16_t;
		using Vector2Type	= sf::Vector2<SizeType>;

	public:
		Component() = default;
		Component(const Vector2Type& size);
		Component(const Vector2Type& size, bool selectable);

		[[nodiscard]] constexpr bool IsSelected() const noexcept;
		[[nodiscard]] constexpr bool IsSelectable() const noexcept;

	public:
		void Select();
		void Unselect();

	public:
		vlx::Event<> Selected;
		vlx::Event<> Unselected;

	private:
		Vector2Type m_size;
		bool		m_selected			{false};
		bool		m_selectable		{false};

		bool		m_parent_active		{true};
	};
}

