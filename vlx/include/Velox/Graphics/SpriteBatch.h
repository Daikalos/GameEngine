#pragma once

#include <SFML/Graphics.hpp>

#include <numeric>

#include <Velox/Utilities.hpp>
#include <Velox/Config.hpp>
#include <Velox/Components/IBatchable.h>
#include <Velox/Components/Sprite.h>
#include <Velox/Components/Transform.h>

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
		struct Triangle
		{
			const sf::Texture*	texture	{nullptr};
			const sf::Shader*	shader	{nullptr};
			float				depth	{0.0f};
		};

		struct BatchInfo
		{
			const sf::Texture*	texture	{nullptr};
			const sf::Shader*	shader	{nullptr};
			std::size_t			count	{0};
		};

	public:
		void SetSortMode(const SortMode sort_mode);

		void AddTriangle(const sf::Vertex& v0, const sf::Vertex& v1, const sf::Vertex& v2, const Transform& transform, const sf::Texture* texture, const sf::Shader* shader, float depth = 0.0f);

		void Batch(const IBatchable& batchable, float depth = 0.0f);
		void Batch(const sf::VertexArray& vertices, sf::PrimitiveType type, const sf::Texture* texture, const sf::Shader* shader, const Transform& tranform, float depth = 0.0f);

		void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

		void Clear();

	private:
		void SortGlyphs() const;
		void CreateBatches() const;

	private:
		std::vector<Triangle>	m_triangles;
		std::vector<BatchInfo>	m_batches;
		sf::VertexArray			m_vertices;
		SortMode				m_sort_mode			{SortMode::Deferred};

		mutable sf::VertexArray m_sorted_vertices;
		mutable bool			m_update_required	{true};
	};
}

