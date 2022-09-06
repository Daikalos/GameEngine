#include <Velox/ECS/Components/Sprite.h>

using namespace vlx;

Sprite::Sprite(const sf::Texture* texture, float depth)
	: m_sprite(*texture), m_depth(depth)
{

}

Sprite::~Sprite()
{

}

const sf::Texture* Sprite::GetTexture() const noexcept
{
	return m_sprite.getTexture();
}
constexpr const float Sprite::GetDepth() const noexcept
{
	return m_depth;
}

void Sprite::SetDepth(float val) noexcept
{
	m_depth = val;
}

void Sprite::Draw()
{

}