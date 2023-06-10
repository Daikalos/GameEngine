#include <Velox/World/Object.h>

using namespace vlx;

const std::string& Object::GetTag() const noexcept
{
	return m_tag;
}

bool Object::GetActive() const noexcept
{
	return m_activeHierarchy;
}

bool Object::GetActiveSelf() const noexcept
{
	return m_activeSelf;
}

void Object::SetTag(const std::string& tag)
{
	m_tag = tag;
}

void Object::SetTag(std::string&& tag)
{
	m_tag = std::move(tag);
}

void Object::SetActive(bool flag)
{
	if (m_activeSelf != flag)
	{
		m_activeSelf = flag;
		m_dirty = true;
	}
}
