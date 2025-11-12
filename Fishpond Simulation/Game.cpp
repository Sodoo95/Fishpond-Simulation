#include "Game.h"

void Game::Start()
{
	switch (currentGameState)
	{
	case GameState::MENU: mainMenu.Start();
		break;
	case GameState::SIMULATION: simulation.Start();
		break;
	}
}

void Game::Update(float deltaTime)
{
	switch (currentGameState)
	{
	case GameState::MENU: mainMenu.Update(deltaTime);
		break;
	case GameState::SIMULATION: simulation.Update(deltaTime);
		break;
	case GameState::EXIT:
		isRunning = false;
		break;
	}

	if (previousGameState != currentGameState)
	{
		previousGameState = currentGameState;
		Start();
	}
}

void Game::Render()
{
	switch (currentGameState)
	{
	case GameState::MENU: mainMenu.Render();
		break;
	case GameState::SIMULATION: simulation.Render();
		break;
	}
}

Game::Game()
{
	InitWindow(windowWidth, windowHeight, "Fish Pond Simulation");
	SetTargetFPS(60);
	Start();
	while (isRunning && !WindowShouldClose())
	{
		Update(GetFrameTime());
		BeginDrawing();
		ClearBackground(Color(10,30,60,255));
		Render();
		EndDrawing();
	}

	CloseWindow();
}


