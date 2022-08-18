#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "InputHandler.h"

#include "../utilities/VectorUtilities.h"
#include "../utilities/NonCopyable.h"

namespace fge
{
	class Camera : public sf::View, NonCopyable
	{
	public:
		Camera() : m_position(0, 0), m_scale(1, 1), m_size(0, 0) {}
		virtual ~Camera() = default;

		virtual void HandleEvent(const sf::Event& event);

		///////////////////////////////////////////////
		// call after poll event
		///////////////////////////////////////////////
		virtual void Update(const InputHandler& input_handler, const sf::RenderWindow& window) = 0;

	public:
		sf::Transform GetViewMatrix() const
		{
			return sf::Transform()
				.translate(m_position)
				.scale(1.0f / m_scale)
				.translate(m_size / -2.0f);
		}

		sf::Vector2f ViewToWorld(const sf::Vector2f& position) const
		{
			return GetViewMatrix() * position;
		}

		sf::Vector2f GetMouseWorldPosition(const sf::RenderWindow& window) const { return ViewToWorld(sf::Vector2f(sf::Mouse::getPosition(window))); }

		sf::Vector2f GetPosition() const { return m_position; }
		sf::Vector2f GetScale() const { return m_scale; }
		sf::Vector2f GetSize() const { return m_size; }

		void SetPosition(const sf::Vector2f& position)
		{
			setCenter(m_position);
			m_position = position;
		}
		void SetScale(const sf::Vector2f& scale)
		{
			setSize(m_size * (1.0f / scale));
			m_scale = scale;
		}
		void SetSize(const sf::Vector2f& size) 
		{ 
			m_size = size;
			setSize(m_size * (1.0f / m_scale));
		}

	protected:
		void SetLetterboxView(int width, int height);

	protected:
		sf::Vector2f m_position;
		sf::Vector2f m_scale;	 
		sf::Vector2f m_size;
	};
}

