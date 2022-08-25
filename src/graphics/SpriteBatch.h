#pragma once

#include <SFML/Graphics.hpp>
#include "../Utilities.hpp"

namespace vlx
{
	class SpriteBatch
	{
	public:
		enum class SortMode
		{
			Deferred,
			BackToFront,
			FrontToBack,
			Texture,
		};

		struct Quad
		{
			Quad() = default;

			const sf::Texture*	m_texture	{nullptr};
			float				m_depth		{0.0f};
			RectFloat			m_rect;
		};

		struct Batch
		{
			Batch() = default;

			const sf::Texture*	m_texture	{nullptr};
			std::size_t			m_count		{0};
			std::size_t			m_offset	{0};
		};

	public:
		SpriteBatch() = default;
		~SpriteBatch() = default;

		void Begin(SortMode sort_mode = SortMode::Deferred);
		void Draw(const RectFloat& dest_rect, const RectFloat& uv_rect, const sf::Texture* texture, float depth = 0.0f);
		void End();

		void Clear();

	private:
		void SortQuads();
		void CreateBatches();

	private:
		std::vector<Batch>		m_batches;
		std::vector<Quad>		m_quads;
		std::vector<sf::Vertex> m_mvertices;

		SortMode				m_sort_mode;
	};
}

