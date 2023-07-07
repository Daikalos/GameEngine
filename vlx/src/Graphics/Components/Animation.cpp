#include <Velox/Graphics/Components/Animation.h>

using namespace vlx;

Animation::Animation() = default;

Animation::Animation(const Vector2u& size, float speed)
	: m_size(size), m_speed(speed) { }

Animation::Animation(uint32 width, uint32 height, float speed)
	: Animation(Vector2u(width, height), speed) { }

Animation::Animation(const Vector2u& size)
	: m_size(size) { }

Animation::Animation(uint32 width, uint32 height)
	: Animation(Vector2u(width, height)) { }

bool Animation::Update(float dt)
{
	uint32 prev = static_cast<uint32>(m_index);

	m_index += au::Wrap(m_speed * dt, static_cast<float>(m_size.x * m_size.y), 0.0f);

	return prev != GetIndex();
}

const Vector2u& Animation::GetSize() const noexcept
{
	return m_size;
}
uint32 Animation::GetIndex() const noexcept
{
	return (uint32)m_index;
}
float Animation::GetSpeed() const noexcept
{
	return m_speed;
}
void Animation::SetSize(const Vector2u& size)
{
	m_size = size;
}

void Animation::SetSize(uint32 width, uint32 height)
{
	SetSize({ width, height });
}
void Animation::SetIndex(uint32 idx)
{
	m_index = (float)idx;
}
void Animation::SetSpeed(float speed)
{
	m_speed = speed;
}
