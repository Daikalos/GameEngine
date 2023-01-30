#pragma once

#include <iostream>

#include <Velox/ECS.hpp>
#include <Velox/Utilities.hpp>
#include <Velox/World/World.h>
#include <Velox/ObjectTypes.hpp>

class StateTest : public vlx::State
{
public:
	using State::State;

	void OnCreated() override;

public:
	bool HandleEvent(const sf::Event& event) override;
	bool Update(vlx::Time& time) override;
	void Draw() override;

private:
	vlx::EntityAdmin* m_entity_admin {nullptr};
	std::vector<vlx::Entity> m_entities;

	vlx::Entity e0, e1, e2;
	vlx::ComponentRef<vlx::LocalTransform> et0, et1;

	vlx::Entity b0, b1;
};