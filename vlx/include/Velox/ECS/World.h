#pragma once

#include <SFML/Graphics.hpp>

#include <Velox/ECS.hpp>

#include <Velox/Systems/ObjectSystem.h>
#include <Velox/Systems/RenderSystem.h>
#include <Velox/Systems/TransformSystem.h>

#include <Velox/Config.hpp>

namespace vlx
{
	/// <summary>
	/// Represents the world's logic e.g., objects, rendering, transforms, etc.
	/// </summary>
	class VELOX_API World : public sf::Drawable
	{
	public:
		World();

		void Run();
		void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

		EntityAdmin& GetEntityAdmin() noexcept;
		ObjectSystem& GetObjectSystem() noexcept;
		TransformSystem& GetTransformSystem() noexcept;
		RenderSystem& GetRenderSystem() noexcept;

		const EntityAdmin& GetEntityAdmin() const noexcept;
		const ObjectSystem& GetObjectSystem() const noexcept;
		const TransformSystem& GetTransformSystem() const noexcept;
		const RenderSystem& GetRenderSystem() const noexcept;

	private:
		EntityAdmin		m_entity_admin;

		ObjectSystem	m_object_system;
		TransformSystem m_transform_system;
		RenderSystem	m_render_system;
	};
}