#pragma once

#include <SFML/Graphics.hpp>
#include "../Utilities.hpp"

namespace fge
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

			const sf::Texture* _texture{nullptr};
			float _depth{ 0.0f };
			RectFloat _rect;
		};

		struct Batch
		{
			Batch() = default;

			const sf::Texture* _texture{ nullptr };
			std::size_t _count{ 0 };
			std::size_t _offset{ 0 };
		};

	public:
		SpriteBatch() = default;
		~SpriteBatch() = default;

		void begin(SortMode sort_mode = SortMode::Deferred);
		void draw(const RectFloat& dest_rect, const RectFloat& uv_rect, const sf::Texture* texture, float depth = 0.0f);
		void end();

		void clear();

	private:
		void sort_quads();
		void create_batches();

	private:
		std::vector<Batch> _batches;
		std::vector<Quad> _quads;
		std::vector<sf::Vertex> _vertices;

		SortMode _sort_mode;
	};
}

