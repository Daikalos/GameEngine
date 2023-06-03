#pragma once

#include <iostream>

#include <Velox/Velox.hpp>

#include "../Cameras/CameraFollow.h"

struct PlayerData
{
	vlx::ComponentRef<vlx::PhysicsBody> body;
	vlx::ComponentRef<vlx::Collider>	collider;
	vlx::ComponentRef<vlx::Transform>	transform;
	bool jump {true};
};

class PlayerBehaviour : public vlx::DataBehaviour<PlayerBehaviour, PlayerData>
{
public:
	PlayerBehaviour(vlx::World& world) 
		: DataBehaviour(world.GetEntityAdmin()), m_keyboard(&world.GetControls().Get<vlx::KeyboardInput>()), m_time(&world.GetTime())
	{

	}

public:
	void Start(vlx::EntityID entity_id, PlayerData& data)
	{
		data.body = GetEntityAdmin()->GetComponentRef<vlx::PhysicsBody>(entity_id);
		data.collider = GetEntityAdmin()->GetComponentRef<vlx::Collider>(entity_id);
		data.transform = GetEntityAdmin()->GetComponentRef<vlx::Transform>(entity_id);

		data.collider->OnEnter += [&data](const vlx::CollisionResult& result)
		{
 			if (result.contacts[0].normal.Dot(vlx::Vector2f::Up) > 0.8f)
				data.jump = true;
		};
	}
	void Update(vlx::EntityID entity_id, PlayerData& data)
	{
		if (m_keyboard->Held(sf::Keyboard::A))
			data.transform->Move({ -50.0f * m_time->GetDT(), 0.0f });

		if (m_keyboard->Held(sf::Keyboard::D))
			data.transform->Move({ 50.0f * m_time->GetDT(), 0.0f });

		if (data.jump && m_keyboard->Pressed(sf::Keyboard::Space))
		{
			data.body->ApplyForceToCenter({ 0.0f, -4000.0f });
			data.body->SetVelocity({});
			data.jump = false;
		}
	}
	void Destroy(vlx::EntityID entity_id, PlayerData& data)
	{
		std::puts("destroy");
	}

private:
	vlx::KeyboardInput* m_keyboard {nullptr};
	vlx::Time* m_time;
};

class StateTest : public vlx::State
{
public:
	StateTest(vlx::StateStack& state_stack, vlx::World& world, vlx::StateID id);

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

	vlx::FPSCounter counter;

	vlx::Entity player;
	PlayerBehaviour ply;

	//vlx::System<vlx::Velocity, vlx::LocalTransform> sys;
};