#include <Velox/Graphics/GUI/Container.h>

#include <Velox/Graphics/GUI/GUIComponent.h>

using namespace vlx::gui;

constexpr bool Container::IsEmpty() const noexcept
{
	return m_children.empty();
}
constexpr bool Container::IsSelectable() const noexcept
{
	return false;
}

bool Container::HasSelection() const noexcept
{
	return m_selected_child < 0;
}

void Container::SelectAt(int index)
{
	if (IsEmpty())
		return;

	if (index < 0 || index >= m_children.size())
	{
		if (HasSelection())
			m_children[m_selected_child]->Get()->Deselect();

		m_selected_child = -1;
	}
	else if (index != m_selected_child && m_children[index]->Get()->IsSelectable())
	{
		if (HasSelection())
			m_children[m_selected_child]->Get()->Deselect();

		m_children[index]->Get()->Select();
		m_selected_child = index;
	}
}

void Container::SelectNext() 
{
	if (!HasSelection())
		return;

	int next = m_selected_child;

	do next = (next + 1) % m_children.size();
	while (!m_children[next]->Get()->IsSelectable());

	SelectAt(next);
}
void Container::SelectPrev()
{
	if (!HasSelection())
		return;

	int prev = m_selected_child;

	do prev = (prev - 1 + m_children.size()) % m_children.size();
	while (!m_children[prev]->Get()->IsSelectable());

	SelectAt(prev);
}
