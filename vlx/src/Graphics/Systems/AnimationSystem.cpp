#include <Velox/Graphics/Systems/AnimationSystem.h>

using namespace vlx;

AnimationSystem::AnimationSystem(EntityAdmin& entity_admin, LayerType layer, const Time& time)
    : SystemAction(entity_admin, layer), m_animation(entity_admin)
{
    m_animation.Each(
        [&time](EntityID entity_id, Renderable& renderable, Sprite& sprite, Animation& animation)
        {
            if (animation.Update(time.GetDT()))
            {
                sprite.SetTextureRect(RectFloat(
                    sprite.GetSize().x * (animation.GetIndex() % animation.GetSize().x),
                    sprite.GetSize().y * (animation.GetIndex() / animation.GetSize().x),
                    sprite.GetSize().x, sprite.GetSize().y));
            }
        });
}

void AnimationSystem::Start()
{
}

void AnimationSystem::PreUpdate()
{
}

void AnimationSystem::Update()
{
    Execute();
}

void AnimationSystem::FixedUpdate()
{
}

void AnimationSystem::PostUpdate()
{
}

void vlx::AnimationSystem::Draw(Window& window) const
{
}

void vlx::AnimationSystem::DrawGUI(Window& window) const
{
}
