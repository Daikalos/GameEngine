#include <Velox/Physics/Collider.h>

using namespace vlx;

bool Collider::GetEnabled() const noexcept
{
	return enabled;
}

void Collider::SetEnabled(bool flag)
{
	if (enabled == flag)
		return;

	if (!flag)
		Erase();

	enabled = flag;
}
