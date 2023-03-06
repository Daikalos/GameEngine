#pragma once

#include <Velox/Scene/State.h>

#include <Velox/System/Time.h>

class StateLoading final : public vlx::State
{
public:
	bool HandleEvent(const sf::Event& event) override;
	bool Update(vlx::Time& time) override;
	void Draw() override;



};