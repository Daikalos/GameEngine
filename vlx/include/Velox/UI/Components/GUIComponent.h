#pragma once

#include <Velox/System/Vector2.hpp>
#include <Velox/System/Event.hpp>
#include <Velox/VeloxTypes.hpp>
#include <Velox/Config.hpp>

namespace vlx::ui
{
	class Container;

	///	Interface for GUI components to inherit from
	/// 
	class VELOX_API GUIComponent
	{
	protected:
		using Vector2Type = Vector2<uint16>;

	public:
		GUIComponent() = default;
		GUIComponent(uint16 width, uint16 height);
		GUIComponent(const Vector2<uint16>& size);

		virtual ~GUIComponent() = 0;

	public:
		NODISC const Vector2<uint16>& GetSize() const noexcept;
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
		Vector2<uint16> m_size;
		bool			m_selected	{false};
		bool			m_active	{true};

		Container*		m_container	{nullptr}; // current container associated with

		friend class Container;
	};
}

