#include <Velox/Graphics/GUI/Text.h>

using namespace vlx::gui;

constexpr bool Text::IsSelectable() const noexcept
{
    return false;
}
