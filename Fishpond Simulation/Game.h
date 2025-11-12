#pragma once
#include <raylib.h>
#include "Globals.h"
#include "MainMenu.h"
#include "Simulation.h"
class Game
{
	void Start();
	void Update(float deltaTime);
	void Render();

	MainMenu mainMenu;
	Simulation simulation;

	GameState previousGameState = GameState::MENU;

public:
	Game();
};

