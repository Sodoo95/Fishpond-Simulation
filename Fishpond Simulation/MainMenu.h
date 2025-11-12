#pragma once
#include <raylib.h>
#include "Globals.h"
#include "UIComponent.h"


class MainMenu
{
private:
	SimpleButton startButton;
	SimpleButton settingsButton;
	SimpleButton exitButton;

	SimpleButton button_800x600;
	SimpleButton button_1024x768;
	SimpleButton button_1366x768;
	SimpleButton backButton;

	bool inSettings = false;

public:
	MainMenu();
	void Start();
	void Update(float deltaTime);
	void Render();
};

