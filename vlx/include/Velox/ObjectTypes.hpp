#pragma once

#include "Utilities.hpp"

#include "Components/Object.h"
#include "Components/Transform.h"
#include "Components/Relation.h"
#include "Components/Sprite.h"

namespace vlx
{
	using ObjectType = std::tuple<Object, Transform, Relation, Sprite>;
}