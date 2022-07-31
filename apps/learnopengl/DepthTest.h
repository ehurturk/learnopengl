#pragma once

#include "Engine.hpp"

class DepthTest : public Application {
public:
	DepthTest();
	~DepthTest();

	// Executed when beginning the app
	void start() override;
	// Executed each frame
	void update(float dt) override;
	// Executed each frame for UI
	void imgui_update() override;
private:
	// All private variables...
};
