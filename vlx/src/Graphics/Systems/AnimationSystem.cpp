#include <Velox/Graphics/Systems/AnimationSystem.h>

using namespace vlx;

AnimationSystem::AnimationSystem(EntityAdmin& entity_admin, LayerType layer, const Time& time)
    : SystemAction(entity_admin, layer), m_time(&time), m_animation(entity_admin)
{
    m_animation.Each(&AnimationSystem::UpdateAnimation, this);
}

void AnimationSystem::Update()
{
    Execute();
}

void AnimationSystem::UpdateAnimation(Renderable& renderable, Sprite& sprite, Animation& animation)
{
    if (animation.Update(m_time->GetDT()))
    {
        sprite.SetTextureRect(RectFloat(
            sprite.GetSize().x * (animation.GetIndex() % animation.GetSize().x),
            sprite.GetSize().y * (animation.GetIndex() / animation.GetSize().x),
            sprite.GetSize().x, sprite.GetSize().y));
    }
}
