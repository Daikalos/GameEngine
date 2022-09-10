#pragma once

#include <SFML/Graphics.hpp>

#include <numeric>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>
#include <Velox/ECS/Components/Sprite.h>

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
			const sf::Texture*	texture	{nullptr};
			float				depth		{0.0f};
		};

		struct BatchInfo
		{
			const sf::Texture*	texture	{nullptr};
			const sf::Shader*	shader	{nullptr};
			std::size_t			count		{0};
		};

	public:
		void SetSortMode(const SortMode sort_mode);

		void AddTriangle(const sf::Vertex& v0, const sf::Vertex& v1, const sf::Vertex& v2, const sf::Transform& transform, const sf::Texture& texture, float depth);

		void Batch(const Sprite& sprite, float depth = 0.0f);

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

