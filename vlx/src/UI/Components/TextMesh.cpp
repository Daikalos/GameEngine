#include <Velox/UI/Components/TextMesh.h>

#include <Velox/Graphics/SpriteBatch.h>

using namespace vlx::ui;

void TextMesh::BatchImpl(SpriteBatch& sprite_batch, const Mat4f& transform, float depth) const
{
	if (m_texture)
	{
		if (m_draw_outline)
			sprite_batch.Batch(transform, m_outline, GetPrimitive(), m_texture, nullptr, m_depth);

		sprite_batch.Batch(transform, m_vertices, GetPrimitive(), m_texture, nullptr, m_depth);
	}
}
