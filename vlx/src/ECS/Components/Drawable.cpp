#include <Velox/ECS/Components/Drawable.h>

using namespace vlx;

Drawable::Drawable(const sf::Texture* texture, float depth)
	: m_sprite(*texture), m_depth(depth)
{

}

Drawable::~Drawable()
{

}

constexpr const sf::Texture* const Drawable::GetTexture() const noexcept
{
	return m_sprite.getTexture();
}
constexpr const float Drawable::GetDepth() const noexcept
{
	return m_depth;
}

void Drawable::SetDepth(float val) noexcept
{
	m_depth = val;
}

void Drawable::Draw()
{

}