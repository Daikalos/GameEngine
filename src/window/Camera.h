#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "InputHandler.h"
#include "Window.h"

namespace fge
{
	class Camera
	{
	public:
		Camera() = delete;
		Camera(const Window* window);

		// call after poll event
		//
		void update(const InputHandler& input_handler);

		template<typename T> sf::Vector2<T> view_to_world(const sf::Vector2<T>& position) const
		{
			return sf::Vector2<T>(get_view_matrix() * sf::Vector2f(position));
		}

	public:
		inline const float* get_world_matrix() const
		{
			return sf::Transform()
				.translate(sf::Vector2f(_window->getSize()) / 2.0f)
				.scale(_scale)
				.translate(sf::Vector2f(-_position)).getMatrix();
		}
		inline sf::Transform get_view_matrix() const
		{
			return sf::Transform()
				.translate(sf::Vector2f(_position))
				.scale(sf::Vector2f(1.0f / _scale.x, 1.0f / _scale.y))
				.translate(sf::Vector2f(_window->getSize()) / -2.0f);
		}

		inline sf::Vector2i get_position() const { return _position; }
		inline sf::Vector2i get_mouse_world_position() const { return view_to_world(sf::Mouse::getPosition(*_window)); }

		inline void set_position(sf::Vector2i position)
		{
			_position = position;
		}
		inline void set_scale(float scale)
		{
			_scale.x = scale;
			_scale.y = scale;
		}

		inline sf::Vector2f get_scale() const { return _scale; }

	private:
		const Window* _window;

		sf::Vector2i _position;
		sf::Vector2f _scale;

		sf::Vector2i _dragPos;
	};
}

