#pragma once

#include <SFML/Graphics.hpp>

#include <numeric>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>
#include <Velox/ECS/Components/Sprite.h>
#include <Velox/ECS/Components/Transform.h>

namespace vlx
{
	class VELOX_API SpriteBatch final : public sf::Drawable
	{
	public:
		enum class SortMode : std::uint16_t
		{
			Deferred,
			BackToFront,
			FrontToBack,
			Texture,
		};

	private:
		struct Glyph
		{
			Glyph(const RectFloat& dest_rect, const RectFloat& uv_rect, const sf::Texture* texture, const sf::Color& color, float depth = 0.0f);
			Glyph(const Sprite& sprite, const Transform& transform, float depth = 0.0f);

			const sf::Texture*	m_texture	{nullptr};
			float				m_depth		{0.0f};
		};

		struct BatchInfo
		{
			const sf::Texture*	m_texture	{nullptr};
			const sf::Shader*	m_shader	{nullptr};
			std::size_t			m_offset	{0};
			std::size_t			m_count		{0};
		};

	public:
		void SetSortMode(const SortMode sort_mode);

		void Batch(const sf::Texture* texture, const RectFloat& dest_rect, const RectFloat& uv_rect, const sf::Color& color, float depth = 0.0f);
		void Batch(const Sprite& sprite, const Transform& transform, float depth = 0.0f);

		void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

		void Clear();

	private:
		void SortGlyphs() const;
		void CreateBatches() const;

	private:
		std::vector<Glyph>		m_glyphs;
		std::vector<BatchInfo>	m_batches;

		sf::VertexArray			m_vertices;
		mutable sf::VertexArray m_sorted_vertices;

		SortMode				m_sort_mode			{SortMode::Deferred};
		mutable bool			m_update_required	{true};
	};
}

