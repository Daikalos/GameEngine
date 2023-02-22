#pragma once

#include <Velox/Utilities.hpp>

#include <Velox/Graphics/Components/Object.h>
#include <Velox/Graphics/Components/LocalTransform.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/Relation.h>
#include <Velox/Graphics/Components/Sprite.h>

#include <Velox/Algorithms/QTElement.hpp>

#include <Velox/Physics/Circle.h>
#include <Velox/Physics/Box.h>
#include <Velox/Physics/Polygon.h>
#include <Velox/Physics/PhysicsBody.h>

#include <Velox/Graphics/GUI/Container.h>
#include <Velox/Graphics/GUI/Button.h>
#include <Velox/Graphics/GUI/Label.h>
#include <Velox/Graphics/GUI/TextBox.h>

// TODO: maybe expand this to return constructed object with default parameters set, e.g., 
// quickly return a button object with sprite, events, and default colors set

namespace vlx
{
	using AllTypes		= std::tuple<Object, LocalTransform, Transform, Relation, Sprite, Circle, Box, Polygon, PhysicsBody, gui::Container, gui::Button, gui::Label>;
	using ObjectType	= std::tuple<Object, LocalTransform, Transform, Relation, Sprite>;

	namespace gui
	{
		using ContainerType = std::tuple<Object, LocalTransform, Transform, Relation, gui::Container>;
		using ImageType		= std::tuple<Object, LocalTransform, Transform, Relation, Sprite>;
		using ButtonType	= std::tuple<Object, LocalTransform, Transform, Relation, Sprite, gui::Button>;
		using LabelType		= std::tuple<Object, LocalTransform, Transform, Relation, gui::Label>;
	}
}