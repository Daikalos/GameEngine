#include <Velox/Graphics/GUI/Container.h>

using namespace vlx::gui;

constexpr bool Container::IsEmpty() const noexcept
{
	return m_children.empty();
}

void Container::Push(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	m_children.push_back(entity_id);
}
void Container::Erase(const EntityAdmin& entity_admin, const EntityID entity_id)
{
	const auto it = std::find(m_children.begin(), m_children.end(), entity_id);
	if (it == m_children.end())
		return;

	const auto index = it - m_children.begin();

	if (m_selected_index >= index)
		SelectPrev(entity_admin);
	
	m_children.erase(it);
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

	SizeType next_index = (SizeType)au::Wrap<int>(m_selected_index + steps, 0, m_children.size() - 1);

	if (next_index == m_selected_index)
		return;

	const EntityID prev_id = m_children[m_selected_index];
	entity_admin.GetComponent<Component>(prev_id).Unselect();

	m_selected_index = next_index;

	const EntityID next_id = m_children[m_selected_index];
	entity_admin.GetComponent<Component>(next_id).Select();
}