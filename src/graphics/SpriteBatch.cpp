#include <Velox/Graphics/SpriteBatch.h>

using namespace vlx;

void SpriteBatch::Begin(SortMode sort_mode)
{
	m_sort_mode = sort_mode;
	m_batches.clear();
	m_quads.clear();
}

void SpriteBatch::End()
{
	SortQuads();
	CreateBatches();
}

void SpriteBatch::SortQuads()
{
	switch (m_sort_mode)
	{
	case SortMode::BackToFront:
		std::stable_sort(m_quads.begin(), m_quads.end(), [](const Quad& q0, const Quad& q1) { return q0.m_depth > q1.m_depth; });
		break;
	case SortMode::FrontToBack:
		std::stable_sort(m_quads.begin(), m_quads.end(), [](const Quad& q0, const Quad& q1) { return q0.m_depth < q1.m_depth; });
		break;
	case SortMode::Texture:
		std::stable_sort(m_quads.begin(), m_quads.end(), [](const Quad& q0, const Quad& q1) { return q0.m_texture < q1.m_texture; });
		break;
	}
}

void SpriteBatch::CreateBatches()
{
	if (!m_quads.empty())
		return;

	//int offset = 0;
	//for (int i = 0; i < _quads.size(); ++i)
	//{
	//	_vertices[]
	//}
}
