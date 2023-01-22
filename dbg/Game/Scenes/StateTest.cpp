#include "StateTest.h"

using namespace vlx;

void StateTest::OnCreated()
{
    m_entity_admin = &GetWorld().GetEntityAdmin();

	ObjectSystem& object_system = GetWorld().GetSystem<ObjectSystem>();

	e0 = object_system.CreateObject();
	e0.AddComponents(ObjectType{});

	e0.GetComponent<Sprite>().SetTexture(GetWorld().GetTextureHolder().Get(Texture::ID::IdleCursor));
	e0.GetComponent<Sprite>().SetOpacity(1.0f);

	et0 = e0.GetComponentRef<Transform>();
	et0->Get()->SetPosition({ 50.0f, 50.0f });

	e1 = e0.Duplicate();
	et1 = e1.GetComponentRef<Transform>();

	GetWorld().GetSystem<RelationSystem>().AttachInstant(e0.GetID(), e1.GetID());

	e2 = e1.Duplicate();
	e2.AddComponent<gui::Label>(105, 105);

	gui::Label& label = e2.GetComponent<gui::Label>();
	label.setString("potato");

	m_entities.reserve(100000);

	m_entity_admin->Reserve<Object, Transform, Relation, Sprite>(m_entities.capacity());
	for (int i = 0; i < m_entities.capacity(); ++i)
	{
		Entity& added = m_entities.emplace_back(e1.Duplicate());
		added.GetComponent<Transform>().SetPosition({ rnd::random() * 10000, rnd::random() * 10000 });
	}
}

bool StateTest::HandleEvent(const sf::Event& event)
{
    return true;
}

bool StateTest::Update(Time& time)
{
	if (m_entity_admin->IsEntityRegistered(e0))
		et0->Get()->Move({ 5.0f * time.GetDT(), 0.0f });

	et1->Get()->SetPosition({ 0.0f, 5.0f * time.GetDT() });

	if (GetWorld().GetControls().Get<KeyboardInput>().Pressed(sf::Keyboard::Space))
		GetWorld().GetSystem<ObjectSystem>().DeleteObjectInstant(e0); // TODO: tell children transforms that parent was removed

	GetWorld().GetWindow().setTitle(std::to_string(test));

    return true;
}

void StateTest::Draw()
{

}
