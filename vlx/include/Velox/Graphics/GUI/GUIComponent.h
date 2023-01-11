#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>

namespace vlx::gui
{
	class GUIComponent : public IComponent
	{
	private:
		using SizeType		= std::uint16_t;
		using Vector2Type	= sf::Vector2<SizeType>;

	public:
		GUIComponent() = default;
		GUIComponent(const Vector2Type& size);
		GUIComponent(const Vector2Type& size, bool selectable);

		virtual ~GUIComponent() = 0;

		[[nodiscard]] constexpr bool IsSelected() const noexcept;
		[[nodiscard]] constexpr bool IsSelectable() const noexcept;

	public:
		void Select();
		void Deselect();

	public:
		vlx::Event<> Selected;
		vlx::Event<> Deselected;

	private:
		Vector2Type m_size;
		bool		m_selected			{false};
		bool		m_selectable		{false};
	};

	inline GUIComponent::~GUIComponent() = default;
}

