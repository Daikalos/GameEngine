#pragma once

#include <tuple>
#include <type_traits>

#include <Velox/Graphics/Components/Renderable.h>
#include <Velox/Graphics/Components/GlobalTransformDirty.h>
#include <Velox/Graphics/Components/GlobalTransformMatrix.h>
#include <Velox/Graphics/Components/GlobalTransformMatrixInverse.h>
#include <Velox/Graphics/Components/GlobalTransformTranslation.h>
#include <Velox/Graphics/Components/GlobalTransformRotation.h>
#include <Velox/Graphics/Components/GlobalTransformScale.h>
#include <Velox/Graphics/Components/Transform.h>
#include <Velox/Graphics/Components/TransformMatrix.h>
#include <Velox/Graphics/Components/TransformMatrixInverse.h>
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
#include <Velox/Physics/PhysicsBodyTransform.h>
#include <Velox/Physics/Collider.h>

#include <Velox/World/Object.h>

// TODO: maybe expand this to return constructed object with default parameters set, e.g., 
// return a button object with sprite, events, and default colors set

namespace vlx
{
	using AllTypes = std::type_identity<std::tuple<
		Object, Renderable, Relation, Sprite, Mesh,
		Transform, TransformMatrix, TransformMatrixInverse, 
		GlobalTransformTranslation, GlobalTransformRotation, GlobalTransformScale,
		GlobalTransformDirty, GlobalTransformMatrix, GlobalTransformMatrixInverse,
		Circle, Box, Collider, Polygon, Point, PhysicsBody, PhysicsBodyTransform,
		gui::Container, gui::Button, gui::Label>>;

	using ObjectType = std::type_identity<std::tuple<
		Object, Renderable, Sprite, Relation,
		Transform, TransformMatrix, TransformMatrixInverse, 
		GlobalTransformTranslation, GlobalTransformDirty, GlobalTransformMatrix, GlobalTransformMatrixInverse>>;

	using PhysicsType = std::type_identity<std::tuple<Collider, PhysicsBody, PhysicsBodyTransform>>;

	namespace gui
	{
		using ContainerType = std::type_identity<std::tuple<Renderable, Transform, GlobalTransformTranslation, Relation, gui::Container>>;
		using ImageType		= std::type_identity<std::tuple<Renderable, Transform, GlobalTransformTranslation, Relation, Sprite>>;
		using ButtonType	= std::type_identity<std::tuple<Renderable, Transform, GlobalTransformTranslation, Relation, Sprite, gui::Button>>;
		using LabelType		= std::type_identity<std::tuple<Renderable, Transform, GlobalTransformTranslation, Relation, gui::Label>>;
	}
}