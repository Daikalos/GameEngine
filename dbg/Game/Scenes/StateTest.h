#pragma once

#include <iostream>

#include <Velox/Velox.hpp>

#include <Velox/Algorithms/LQuadTree.hpp>

class StateTest : public vlx::State
{
public:
	StateTest(const vlx::StateID state_id, vlx::StateStack& state_stack, vlx::World& world);

	void OnCreated() override;

public:
	bool HandleEvent(const sf::Event& event) override;
	bool Update(vlx::Time& time) override;
	void Draw() override;

private:
	vlx::EntityAdmin* m_entity_admin {nullptr};
	std::vector<vlx::Entity> m_entities;

	vlx::Entity e0, e1, e2;
	vlx::ComponentRef<vlx::Transform> et0, et1;

	vlx::Entity b0, b1;

	//vlx::System<vlx::Velocity, vlx::LocalTransform> sys;
};