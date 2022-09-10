#pragma once

#include <SFML/Graphics.hpp>

#include "../../utilities/NonCopyable.h"

namespace vlx
{
	namespace gui
	{
		class Component : public sf::Drawable, public sf::Transformable, private NonCopyable
		{
		public:
			using Ptr = typename std::shared_ptr<Component>;

		public:
			Component() = default;
			virtual ~Component() = default;

			[[nodiscard]] virtual bool IsSelectable() const = 0;

			[[nodiscard]] bool IsSelected() const
			{
				return m_selected;
			}
			virtual void Select()
			{
				m_selected = true;
			}
			virtual void Deselect()
			{
				m_selected = false;
			}

			[[nodiscard]] virtual bool IsActive() const
			{
				return m_active;
			}
			virtual void Activate()
			{
				m_active = true;
			}
			virtual void Deactivate()
			{
				m_active = false;
			}

			virtual void CaptureMouseWhenActive() { }

			virtual bool Update(float dt) { return true; }
			virtual void HandleEvent(const sf::Event& event) = 0;
			virtual bool Draw() { return true; }

			virtual bool HandleEventGently() { return false; }
			virtual bool Contains(sf::Vector2i pos) { return false; }
			virtual void SetHover(bool flag) { }
			virtual void HoveredAt(sf::Vector2i pos) { }
			virtual void ClickedAt(sf::Vector2i pos) { }

			virtual sf::Vector2f GetSize() const
			{
				return sf::Vector2f();
			}

		private:
			bool m_selected	{false};
			bool m_active	{false};
		};
	}
}

