#include <Velox/UI/Components/UIBase.h>

using namespace vlx::ui;

auto UIBase::GetSize() const -> const Vector2Type&
{
	return m_size;
}

void UIBase::SetSize(const Vector2Type& size)
{
	m_size = size;
}
