#pragma once

#include "Utilities.hpp"

#include "Components/Object.h"
#include "Components/Transform.h"
#include "Components/Relation.h"
#include "Components/Sprite.h"

#include "Graphics/GUI/Container.h"
#include "Graphics/GUI/Button.h"
#include "Graphics/GUI/Label.h"
#include "Graphics/GUI/TextBox.h"

namespace vlx
{
	using ObjectType	= std::tuple<Object, Transform, Relation, Sprite>;

	namespace gui
	{
		using ContainerType = std::tuple<Object, Transform, Relation, gui::Container>;
		using ImageType		= std::tuple<Object, Transform, Relation, Sprite>;
		using ButtonType	= std::tuple<Object, Transform, Relation, Sprite, gui::Button>;
		using LabelType		= std::tuple<Object, Transform, Relation, gui::Label>;
	}
}