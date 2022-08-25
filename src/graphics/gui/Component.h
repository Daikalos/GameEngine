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
			typedef std::shared_ptr<Component> ptr;

		public:
			Component() { }
			virtual ~Component() { }

			virtual bool is_selectable() const = 0;
			bool is_selected() const
			{
				return _is_selected;
			}

			virtual void select()
			{
				_is_selected = true;
			}
			virtual void deselect()
			{
				_is_selected = false;
			}

			virtual bool is_active() const
			{
				return _is_active;
			}
			virtual void activate()
			{
				_is_active = true;
			}
			virtual void deactivate()
			{
				_is_active = false;
			}

			virtual void capture_mouse_when_active() { }

			virtual bool update(float dt) { return true; }
			virtual void handle_event(const sf::Event& event) = 0;
			virtual bool draw() { return true; }

			virtual bool handle_event_gently() { return false; }
			virtual bool contains(sf::Vector2i pos) { return false; }
			virtual void set_hover(bool flag) { }
			virtual void hovered_at(sf::Vector2i pos) { }
			virtual void clicked_at(sf::Vector2i pos) { }

			virtual sf::Vector2f get_size() const
			{
				return sf::Vector2f();
			}

		private:
			bool _is_selected{ false };
			bool _is_active{ false };
		};
	}
}

