#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>

namespace vlx::gui
{
	class Container;

	/// <summary>
	///		Interface for GUI components to inherit from
	/// </summary>
	class VELOX_API GUIComponent : public IComponent
	{
	private:
		using SizeType		= std::uint16_t;
		using Vector2Type	= sf::Vector2<SizeType>;

	public:
		GUIComponent() = default;
		GUIComponent(const Vector2Type& size);
		GUIComponent(const SizeType width, const SizeType height);

		virtual ~GUIComponent() = 0;

	public:
		[[nodiscard]] auto GetSize() const noexcept -> const Vector2Type&;
		[[nodiscard]] constexpr bool IsActive() const noexcept;
		[[nodiscard]] constexpr bool IsSelected() const noexcept;

		void SetSize(const Vector2Type& size);

		virtual constexpr bool IsSelectable() const noexcept = 0;

	public:
		void Activate();
		void Deactivate();

		void Select();
		void Deselect();

	public:
		vlx::Event<> Activated;
		vlx::Event<> Deactivated;

		vlx::Event<> Selected;
		vlx::Event<> Deselected;

	private:
		Vector2Type m_size;
		bool		m_selected		{false};
		bool		m_active		{false};

		Container*	m_container		{nullptr}; // current container associated with

		friend class Container;
	};
}

