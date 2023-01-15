#include <Velox/Graphics/GUI/Label.h>

using namespace vlx::gui;

constexpr bool Label::IsSelectable() const noexcept
{
    return false;
}
