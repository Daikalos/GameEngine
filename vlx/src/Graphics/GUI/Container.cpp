#include <Velox/Graphics/GUI/Container.h>

using namespace vlx::gui;

void Container::Push(const EntityID entity_id)
{

}
void Container::Erase(const EntityID entity_id)
{

}

void Container::SelectNext(const EntityAdmin& entity_admin) 
{
	if (!HasChildren())
		return;

	m_selected_index = (SizeType)au::Wrap<int>(m_selected_index + 1, 0, m_children.size() - 1);

	const EntityID child_id = m_children[m_selected_index];
	entity_admin.GetComponent<Component>(child_id).Select();
}
void Container::SelectPrev(const EntityAdmin& entity_admin)
{
	if (!HasChildren())
		return;

	m_selected_index = (SizeType)au::Wrap<int>((int)m_selected_index - 1, 0, m_children.size() - 1);

	const EntityID child_id = m_children[m_selected_index];
	entity_admin.GetComponent<Component>(child_id).Unselect();
}