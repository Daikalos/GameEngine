#include <Velox/UI/Components/UIRenderable.h>

using namespace vlx::ui;

auto UIRenderable::GetSize() -> const Vector2Type&
{
	return m_size;
}
