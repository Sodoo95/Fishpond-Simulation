#pragma once
#include <raylib.h>
#include <entt/entt.hpp>
#include <raymath.h>
#include "EntityComponents.h"
#include "Globals.h"
#include "SmallFish.h"
#include "Utils.h"

#include <vector>
#include <cstring>
#include <iostream>





class Simulation
{
private:
	void UpdateSpatialGrid();
	void FindNeighbors();
	void UpdateFishMovement(float deltaTime);
	void UpdateKoiChain(float deltaTime);
	bool LoadTextures();

	entt::registry registry;
	Camera2D camera = { 0 };
	Vector2 dragStart = { 0 };
	bool isDragging = false;

	Vector2 mapSize = { 10000,10000 };
	int cellSize = 200;
	int borderSize = 80;

	Texture2D fishTexture = { 0 };

	std::vector<std::vector<CellPosDir>> spatialGrid;
	float fishSpeed = 200.0f;
	float fishCoherence = 200.0f;
	float fishSeperation = 20.0f;
	float fishCushion = 60.0f;
	float fishSteerSpeed = 6.0f;
	Vector2 fishAvoidance{ 0,0 };

	int cols = 0;
	int rows = 0;

	int dropDownActive = 0;
	bool dropDownEditMode = false;

public:
	Simulation();
	~Simulation() {
		UnloadTexture(fishTexture);
	}
	void Init();
	void Start();
	void Update(float deltaTime);
	void Render();
};

