#include "MainMenu.h"
#include <raylib.h>
#include <stdio.h>


MainMenu::MainMenu()
{
	startButton.pos = { windowWidth / 2.0f, windowHeight / 2.0f };
	settingsButton.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 70 };
	exitButton.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 140 };

	button_800x600.pos = { windowWidth / 2.0f, windowHeight / 2.0f - 30 };
	button_1024x768.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 40 };
	button_1366x768.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 110 };
	backButton.pos = { 100, float(windowHeight - 50) };

}
void MainMenu::Start()
{	
}
void MainMenu::Update(float deltaTime)
{
	if (inSettings) {
		if (button_800x600.isClicked()) {
			windowWidth = 800;
			windowHeight = 600;
			SetWindowSize(windowWidth, windowHeight);

			button_800x600.pos = { windowWidth / 2.0f, windowHeight / 2.0f - 30 };
			button_1024x768.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 40 };
			button_1366x768.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 110 };
			backButton.pos = { 100, float(windowHeight - 50) };
		}
		if (button_1024x768.isClicked()) {
			windowWidth = 1024;
			windowHeight = 768;
			SetWindowSize(windowWidth, windowHeight);

			button_800x600.pos = { windowWidth / 2.0f, windowHeight / 2.0f - 30 };
			button_1024x768.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 40 };
			button_1366x768.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 110 };
			backButton.pos = { 100, float(windowHeight - 50) };
		}
		if (button_1366x768.isClicked()) {
			windowWidth = 1366;
			windowHeight = 768;
			SetWindowSize(windowWidth, windowHeight);

			button_800x600.pos = { windowWidth / 2.0f, windowHeight / 2.0f - 30 };
			button_1024x768.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 40 };
			button_1366x768.pos = { windowWidth / 2.0f, windowHeight / 2.0f + 110 };
			backButton.pos = { 100, float(windowHeight - 50) };
		}
		if( backButton.isClicked()) {
			inSettings = false;
		}
	}
	else {
		if (startButton.isClicked()) currentGameState = GameState::SIMULATION;
		if (settingsButton.isClicked()) inSettings = true;
		if (exitButton.isClicked()) currentGameState = GameState::EXIT;
	}
}
void MainMenu::Render()
{ 

	if (inSettings) {
		DrawText("Settings", windowWidth / 2 - MeasureText("Settings", 40) / 2, windowHeight / 4 - 40, 40, WHITE);
		DrawText("Window Size", windowWidth / 2 - MeasureText("WindowSize", 40) / 2, windowHeight / 2.0f - 100, 40, WHITE);
		button_800x600.draw("800x600", 24);
		button_1024x768.draw("1024x768", 24);
		button_1366x768.draw("1366x768", 24);
		backButton.draw("Back", 24);
	}
	else {
		DrawText("Fish Pond Simulation", windowWidth / 2 - MeasureText("Fish Pond Simulation", 40) / 2, windowHeight / 4 - 40, 40, WHITE);
		startButton.draw("Start", 24);
		settingsButton.draw("Settings", 24);
		exitButton.draw("Exit", 24);
	}
	
	
	
}
