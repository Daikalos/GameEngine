#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Event.hpp>
#include <Velox/VeloxTypes.hpp>
#include <Velox/Config.hpp>

namespace vlx::gui
{
	class Container;

	///	Interface for GUI components to inherit from
	/// 
	class VELOX_API GUIComponent
	{
	protected:
		using SizeType		= uint16;
		using Vector2Type	= Vector2<SizeType>;

	public:
		GUIComponent() = default;
		GUIComponent(const Vector2Type& size);
		GUIComponent(const SizeType width, const SizeType height);

		virtual ~GUIComponent() = 0;

	public:
		NODISC auto GetSize() const noexcept -> const Vector2Type&;
		NODISC bool IsActive() const noexcept;
		NODISC bool IsSelected() const noexcept;

		void SetSize(const Vector2Type& size);

		virtual bool IsSelectable() const noexcept = 0;

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

	protected:
		Vector2Type m_size;
		bool		m_selected		{false};
		bool		m_active		{false};

		Container*	m_container		{nullptr}; // current container associated with

		friend class Container;
	};
}

