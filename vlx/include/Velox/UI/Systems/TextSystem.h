#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Glyph.hpp>

#include <Velox/ECS/SystemAction.h>
#include <Velox/ECS/System.hpp>

#include <Velox/UI/Components/Text.h>
#include <Velox/UI/Components/TextMesh.h>

#include <Velox/Config.hpp>

namespace vlx::ui
{
	class VELOX_API TextSystem final : public SystemAction
	{
	public:
		TextSystem(EntityAdmin& entity_admin, LayerType layer);

	public:
		void Update() override;

	private:
		static void AddLine(
			std::vector<sf::Vertex>& m_vertices,
			float line_length,
			float line_top,
			sf::Color color,
			float offset,
			float thickness,
			float outline_thickness = 0.0f);

		static void AddGlyphQuad(
			std::vector<sf::Vertex>& m_vertices, 
			sf::Color color, 
			const sf::Glyph& glyph, 
			float italic_shear);

	private:

	};
}