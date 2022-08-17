#include "SpriteBatch.h"

using namespace fge;

void SpriteBatch::begin(SortMode sort_mode)
{
	_sort_mode = sort_mode;
	_batches.clear();
	_quads.clear();
}

void SpriteBatch::end()
{
	sort_quads();
	create_batches();
}

void SpriteBatch::sort_quads()
{
	switch (_sort_mode)
	{
	case SortMode::BackToFront:
		std::stable_sort(_quads.begin(), _quads.end(), [](const Quad& q0, const Quad& q1) { return q0._depth > q1._depth; });
		break;
	case SortMode::FrontToBack:
		std::stable_sort(_quads.begin(), _quads.end(), [](const Quad& q0, const Quad& q1) { return q0._depth < q1._depth; });
		break;
	case SortMode::Texture:
		std::stable_sort(_quads.begin(), _quads.end(), [](const Quad& q0, const Quad& q1) { return q0._texture < q1._texture; });
		break;
	}
}

void SpriteBatch::create_batches()
{
	if (!_quads.empty())
		return;

	//int offset = 0;
	//for (int i = 0; i < _quads.size(); ++i)
	//{
	//	_vertices[]
	//}
}
