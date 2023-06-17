#pragma once

#include <tuple>
#include <type_traits>

#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/Graphics/Components/GlobalTransform.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/Relation.h>
#include <Velox/Graphics/Components/Sprite.h>
#include <Velox/Graphics/Components/Mesh.h>

#include <Velox/Graphics/GUI/Container.h>
#include <Velox/Graphics/GUI/Button.h>
#include <Velox/Graphics/GUI/Label.h>
#include <Velox/Graphics/GUI/TextBox.h>

#include <Velox/Algorithms/QTElement.hpp>

#include <Velox/Physics/Shapes/Circle.h>
#include <Velox/Physics/Shapes/Box.h>
#include <Velox/Physics/Shapes/Polygon.h>
#include <Velox/Physics/Shapes/Point.h>
#include <Velox/Physics/PhysicsBody.h>
#include <Velox/Physics/Collider.h>

#include <Velox/World/Object.h>

// TODO: maybe expand this to return constructed object with default parameters set, e.g., 
// quickly return a button object with sprite, events, and default colors set

namespace vlx
{
	using AllTypes = std::type_identity<std::tuple<
		Object, Renderable, Transform, GlobalTransform, Relation, 
		Sprite, Mesh, Circle, Box, Collider, Polygon, Point, PhysicsBody, 
		gui::Container, gui::Button, gui::Label>>;

	using ObjectType = std::type_identity<std::tuple<Object, Renderable, Transform, GlobalTransform, Relation, Sprite>>;

	namespace gui
	{
		using ContainerType = std::type_identity<std::tuple<Renderable, Transform, GlobalTransform, Relation, gui::Container>>;
		using ImageType		= std::type_identity<std::tuple<Renderable, Transform, GlobalTransform, Relation, Sprite>>;
		using ButtonType	= std::type_identity<std::tuple<Renderable, Transform, GlobalTransform, Relation, Sprite, gui::Button>>;
		using LabelType		= std::type_identity<std::tuple<Renderable, Transform, GlobalTransform, Relation, gui::Label>>;
	}
}