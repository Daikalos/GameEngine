#include <Velox/World/Systems/GUISystem.h>

using namespace vlx::gui;

void GUISystem::Update()
{

}

constexpr vlx::LayerType GUISystem::GetID() const noexcept
{
    return LayerType();
}
