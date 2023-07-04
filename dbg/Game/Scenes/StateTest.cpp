#include "StateTest.h"

using namespace vlx;

StateTest::StateTest(StateStack& state_stack, World& world, StateID id)
	: State(state_stack, world, id), ply(world) //, sys(world.GetEntityAdmin(), 100)
{

}

void StateTest::OnCreated()
{
    m_entity_admin = &GetWorld().GetEntityAdmin();

	ObjectSystem& object_system = GetWorld().GetSystem<ObjectSystem>();

	e0 = object_system.CreateEntity();
	e0.AddComponents(ObjectType{});

	e0.GetComponent<Sprite>().SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::IdleCursor));
	e0.GetComponent<Sprite>().SetOpacity(1.0f);

	et0 = e0.GetComponentRef<Transform>();
	et0->SetPosition({ 0.0f, 0.0f });

	e1 = e0.Duplicate();
	et1 = e1.GetComponentRef<Transform>();

	GetWorld().GetSystem<RelationSystem>().Attach(e0, e1, RelationSystem::S_Instant);

	Entity e4 = e1.Duplicate();

	e2 = e1.Duplicate();
	e2.AddComponent<gui::Label>(105, 105);

	//gui::Label& label = e2.GetComponent<gui::Label>();
	//label.setString("potato");

	m_entities.reserve(1);

	int h = sizeof(Renderable);

	m_entity_admin->Reserve(m_entities.capacity(), ObjectType{});
	for (int i = 0; i < m_entities.capacity(); ++i)
	{
		Entity& added = m_entities.emplace_back(e1.Duplicate());

		//added.AddComponent<Velocity>();
		//added.SetComponent<Velocity>(
		//	rnd::random(-50.0f, 50.0f),
		//	rnd::random(-50.0f, 50.0f));

		added.GetComponent<Transform>().SetPosition({ rnd::random() * 100, rnd::random() * 100 });
		GetWorld().GetSystem<RelationSystem>().Attach(m_entities.at(rnd::random<int>(0, m_entities.size() - 1)), added, RelationSystem::S_Instant);
	}
	
	m_entity_admin->SortComponents<Relation>(m_entities.front(),
		[](const Relation& lhs, const Relation& rhs)
		{
			if (lhs.HasParent() && rhs.HasParent())
				return lhs.GetParent().entity_id > rhs.GetParent().entity_id;

			return false;
		});

	constexpr int a = sizeof(Sprite);
	constexpr int c = sizeof(Transform);
	constexpr int d = sizeof(GlobalTransformMatrix);
	constexpr int e = sizeof(Relation);

	b0 = object_system.CreateEntity();
	b0.AddComponents(gui::ButtonType{});

	b0.GetComponent<Renderable>().IsGUI = true;
	b0.GetComponent<gui::Button>().SetSize({ 128, 128 });
	b0.GetComponent<Sprite>().SetSize({ 128, 128 });
	b0.GetComponent<Transform>().SetPosition({ 100, 100 });
	b0.GetComponent<Sprite>().SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::Square));
	b0.GetComponent<gui::Button>().OnPress		+= []() { std::puts("Pressed"); };
	b0.GetComponent<gui::Button>().OnRelease	+= []() { std::puts("Relased"); };
	b0.GetComponent<gui::Button>().OnEnter		+= []() { std::puts("Entered"); };
	b0.GetComponent<gui::Button>().OnExit		+= []() { std::puts("Exited"); };

	m_entity_admin->Shrink(true);

	//LQuadTree quad_tree({ 0,0, 1280, 1280 });
	//for (int i = 0; i < 8192 * 2 * 2 * 2 * 2; ++i)
	//{
	//	float x = rnd::random() * 1024.0f;
	//	float y = rnd::random() * 1024.0f;

	//	float w = 1.0f + rnd::random() * 128.0f;
	//	float h = 1.0f + rnd::random() * 128.0f;

	//	quad_tree.Insert(LQuadTree<int>::Element(i, RectFloat(x, y, x + w, y + h)));
	//}

	//quad_tree.Cleanup();

	//auto test = quad_tree.Query({0,0, 5, 5});
	//int sdadsa = test.size();

	Entity& entity = m_entities.emplace_back(e0.Duplicate());
	entity.AddComponents(PhysicsType{});
	entity.AddComponent<Box>(au::Pow(2, 16), 16);

	//entity.GetComponent<Circle>().radius = 32.0f;
	entity.GetComponent<PhysicsBody>().SetType(BodyType::Static);
	entity.GetComponent<Sprite>().SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::Square));
	entity.GetComponent<Sprite>().SetSize({ au::Pow(2, 16), 16 });
	entity.GetComponent<Transform>().SetOrigin({ au::Pow(2, 16) / 2, 8 });
	entity.GetComponent<Transform>().SetRotation(sf::degrees(0.0f));

	constexpr int abc = sizeof(PlayerBehaviour);
	constexpr int abc123 = sizeof(System<Object>);

	GetWorld().GetSystem<TransformSystem>().SetGlobalPosition(entity, {0, 0});

	GetWorld().RemoveSystem<TransformSystem>();

	vlx::Vector2f size(16.0f, 64.0f);

	player = e0.Duplicate();
	player.AddComponents(PhysicsType{});
	player.AddComponents<PlayerData>();
	player.AddComponent<Box>(size);
	player.AddComponents<vlx::ColliderOverlap, vlx::ColliderExit>();
	GetWorld().GetSystem<RelationSystem>().Detach(player.GetComponent<Relation>().GetParent().entity_id, player, RelationSystem::S_Instant);

	//entity.GetComponent<Circle>().radius = 32.0f;
	player.GetComponent<PhysicsBody>().SetMass(1.0f);
	player.GetComponent<PhysicsBody>().SetMaterial({ 1.0f, 0.0f });
	player.GetComponent<PhysicsBody>().SetFixedRotation(true);
	player.GetComponent<PhysicsBody>().SetType(BodyType::Dynamic);
	player.GetComponent<PhysicsBody>().SetGravityScale(1.5f);
	player.GetComponent<PhysicsBody>().SetFriction(100000.0f);
	player.GetComponent<PhysicsBody>().SetSleepingAllowed(false);
	player.GetComponent<Sprite>().SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::Square));
	player.GetComponent<Sprite>().SetSize(size);
	player.GetComponent<Sprite>().SetColor(sf::Color::Blue);
	player.GetComponent<Transform>().SetOrigin(size / 2.0f);

	GetWorld().GetSystem<TransformSystem>().SetGlobalPosition(player, { 0, -64 });

	GetWorld().GetCamera().Push(2);
	GetWorld().GetCamera().ApplyPendingChanges();

	static_cast<CameraFollow*>(GetWorld().GetCamera().GetBehavior(2))->SetTarget(*m_entity_admin, player);
	static_cast<CameraFollow*>(GetWorld().GetCamera().GetBehavior(2))->SetOffset(size / 2.0f);
}

bool StateTest::HandleEvent(const sf::Event& event)
{
    return true;
}

bool StateTest::Update(Time& time)
{
	counter.Update(time);

	if (m_entity_admin->IsEntityRegistered(e0))
		et0->Move({ 5.0f * time.GetDT(), 0.0f });

	if (et1) et1->SetPosition({ 0.0f, 5.0f * time.GetDT() });

	if (GetWorld().GetControls().Get<KeyboardInput>().Pressed(sf::Keyboard::Num1))
	{
		float diameter = 8.0f + rnd::random(0.0f, 128.0f);
		float radius = diameter / 2.0f;

		Entity& entity = m_entities.emplace_back(e0.Duplicate());
		entity.AddComponents(PhysicsType{});
		entity.AddComponent<Circle>(radius);

		entity.GetComponent<PhysicsBody>().SetMass(5.0f + rnd::random(0.0f, 15.0f));
		entity.GetComponent<PhysicsBody>().SetInertia(500.0f + rnd::random(0.0f, 1000.0f));
		entity.GetComponent<Transform>().SetOrigin({ radius, radius });
		entity.GetComponent<Sprite>().SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::Circle));
		entity.GetComponent<Sprite>().SetSize({ diameter, diameter });
		entity.GetComponent<Sprite>().SetColor(sf::Color(rnd::random(0, 255), rnd::random(0, 255), rnd::random(0, 255)));

		GetWorld().GetSystem<TransformSystem>().SetGlobalPosition(entity,
			GetWorld().GetCamera().GetMouseWorldPosition(GetWorld().GetWindow()));
	}

	if (GetWorld().GetControls().Get<KeyboardInput>().Pressed(sf::Keyboard::Num2))
	{
		vlx::Vector2f size(
			8.0f + rnd::random(0.0f, 64.0f),
			8.0f + rnd::random(0.0f, 128.0f));

		Entity& entity = m_entities.emplace_back(e0.Duplicate());
		entity.AddComponents(PhysicsType{});
		entity.AddComponent<Box>(size);

		//entity.GetComponent<Circle>().radius = 32.0f;
		entity.GetComponent<PhysicsBody>().SetMass(5.0f + rnd::random(0.0f, 15.0f));
		entity.GetComponent<PhysicsBody>().SetInertia(500.0f + rnd::random(0.0f, 1000.0f));
		entity.GetComponent<Transform>().SetRotation(sf::radians(rnd::random(0.0f, 3.14f)));
		entity.GetComponent<Sprite>().SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::Square));
		entity.GetComponent<Sprite>().SetSize(size);
		entity.GetComponent<Sprite>().SetColor(sf::Color(rnd::random(0, 255), rnd::random(0, 255), rnd::random(0, 255)));
		entity.GetComponent<Transform>().SetOrigin(size / 2.0f);

		GetWorld().GetSystem<TransformSystem>().SetGlobalPosition(entity,
			GetWorld().GetCamera().GetMouseWorldPosition(GetWorld().GetWindow()));
	}

	if (GetWorld().GetControls().Get<KeyboardInput>().Pressed(sf::Keyboard::Num3))
	{
		Entity& entity = m_entities.emplace_back(e0.Duplicate());
		entity.AddComponents(PhysicsType{});
		entity.RemoveComponent<Sprite>();

		std::vector<Vector2f> vertices;
		vertices.resize(rnd::random(3, 24));

		for (int i = 0; i < vertices.size(); ++i)
		{
			vertices[i] = Vector2f(
				rnd::random(-100.0f, 100.0f),
				rnd::random(-100.0f, 100.0f));
		}

		entity.AddComponent<vlx::Polygon>(vertices);
		Mesh* mesh = entity.AddComponent<Mesh>();

		vlx::Polygon* poly = &entity.GetComponent<vlx::Polygon>(); // have to get component since it changed location in memory

		mesh->Assign(poly->GetVertices());

		entity.GetComponent<PhysicsBody>().SetMass(5.0f + rnd::random(0.0f, 15.0f));
		entity.GetComponent<PhysicsBody>().SetInertia(500.0f + rnd::random(0.0f, 1000.0f));
		//entity.GetComponent<Transform>().SetOrigin(poly->GetLocalCenter());
		mesh->SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::White));
		mesh->SetColor(sf::Color(rnd::random(0, 255), rnd::random(0, 255), rnd::random(0, 255)));

		GetWorld().GetSystem<TransformSystem>().SetGlobalPosition(entity,
			GetWorld().GetCamera().GetMouseWorldPosition(GetWorld().GetWindow()));
	}

	if (GetWorld().GetControls().Get<KeyboardInput>().Pressed(sf::Keyboard::Num4))
	{
		for (int i = 0; i < 50; ++i)
		{
			Entity& entity = m_entities.emplace_back(e0.Duplicate());
			entity.AddComponents(PhysicsType{});
			entity.AddComponent<Point>();

			entity.GetComponent<PhysicsBody>().SetMass(0.1f);
			entity.GetComponent<PhysicsBody>().SetFixedRotation(true);
			entity.GetComponent<Transform>().SetOrigin({ 2, 2 });
			entity.GetComponent<Sprite>().SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::Circle));
			entity.GetComponent<Sprite>().SetSize({ 4, 4 });
			entity.GetComponent<Sprite>().SetColor(sf::Color(rnd::random(0, 255), rnd::random(0, 255), rnd::random(0, 255)));

			GetWorld().GetSystem<TransformSystem>().SetGlobalPosition(entity,
				GetWorld().GetCamera().GetMouseWorldPosition(GetWorld().GetWindow()) + sf::Vector2f(-125.0f + i * 5, 0.0f));
		}
	}

	if (GetWorld().GetControls().Get<KeyboardInput>().Pressed(sf::Keyboard::Backspace))
	{
		std::vector<EntityID> ents = m_entity_admin->GetEntitiesWith<Collider>();

		if (ents.size() > 3)
			m_entity_admin->RemoveEntity(ents[rnd::random(3LLU, ents.size() - 1)]);
	}

	//if (GetWorld().GetControls().Get<KeyboardInput>().Pressed(sf::Keyboard::Space))
	//{
	//	GetWorld().GetSystem<ObjectSystem>().RemoveEntity(player); // TODO: tell children transforms that parent was removed
	//}

	GetWorld().GetWindow().setTitle(std::to_string((int)counter.GetFPS()));

    return true;
}

void StateTest::Draw()
{

}
