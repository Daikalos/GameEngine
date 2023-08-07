#include <Velox/Physics/Collider/Collider.h>

using namespace vlx;

bool Collider::GetEnabled() const noexcept
{
	return enabled;
}

void Collider::SetEnabled(bool flag)
{
	enabled = flag;
}
