#include <Velox/Graphics/GUI/Container.h>

#include <Velox/Graphics/GUI/GUIComponent.h>

using namespace vlx::gui;

constexpr bool Container::IsEmpty() const noexcept
{
	return m_children.empty();
}

void Container::SelectNext(const EntityAdmin& entity_admin) 
{
	SelectSteps(entity_admin, 1);
}
void Container::SelectPrev(const EntityAdmin& entity_admin)
{
	SelectSteps(entity_admin , -1);
}

void Container::SelectSteps(const EntityAdmin& entity_admin, int steps)
{
	if (IsEmpty())
		return;

	SizeType next_index = (SizeType)au::Wrap((int)m_selected_index + steps, 0, (int)m_children.size() - 1);

	if (next_index == m_selected_index)
		return;

	//const EntityID prev_id = m_children[m_selected_index];
	////entity_admin.GetComponent<GUIComponent>(prev_id).Unselect();

	//m_selected_index = next_index;

	//const EntityID next_id = m_children[m_selected_index];
	////entity_admin.GetComponent<GUIComponent>(next_id).Select();
}