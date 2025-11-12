#include "Simulation.h"
#include <raylib.h>
#include <raygui.h>
#include <entt/entt.hpp>
#include "EntityComponents.h"
#include "Globals.h"


void Simulation::UpdateSpatialGrid()
{	
	int totalCells = cols * rows;

	spatialGrid.clear();
	spatialGrid.resize(totalCells);

	auto view = registry.view<PositionComponent, CellID, ForwardDirection>();
	for (auto [entity, pos, cid, fDir] : view.each()) {

		int cellX = (int)Clamp(pos.pos.x / cellSize, 0, (float)cols - 1);
		int cellY = (int)Clamp(pos.pos.y / cellSize, 0, (float)rows - 1);
		int newCellID = cellX + cellY * cols;

		registry.patch<CellID>(entity, [newCellID](auto& c) {
			c.id = newCellID;
			});

		spatialGrid[newCellID].push_back({ pos.pos, fDir.dir });
	}
}

void Simulation::FindNeighbors()
{
	static int frameCounter = 0;
	frameCounter++;
	if (frameCounter % 2 != 0) return; // only update every 2nd frame to save performance

	auto view = registry.view<PositionComponent, CellID, Neighbours, ForwardDirection, NeiDirection>();
	for (auto [entity, pos, cid, neighs, fDir, nDir] : view.each()) {
		neighs.pos.clear();
		nDir.dir.clear();
		float fishCoh = fishCoherence;


		auto neighbourCells = GetNeighbours(cid.id, cols, rows);
		neighbourCells.push_back(cid.id); // include own cell
		for (int n : neighbourCells) {
			for (auto& otherEntity : spatialGrid[n]) {
				if (Vector2DistanceSqr(pos.pos, otherEntity.pos) == 0) continue;
				
				float distSq = Vector2DistanceSqr(pos.pos, otherEntity.pos);
				if (distSq < fishCoh * fishCoh) {
					neighs.pos.push_back(otherEntity.pos);
					nDir.dir.push_back(otherEntity.dir);
				}
			}
		}
	}
}

void Simulation::UpdateFishMovement(float deltaTime)
{
	auto view = registry.group<PositionComponent, RotationComponent, ForwardDirection, Neighbours, NeiDirection, FishSpeed>();
	std::vector<entt::entity> entities(view.begin(), view.end());

#pragma omp parallel for schedule(static)
	for (size_t i = 0; i < entities.size(); i++) {
		auto entity = entities[i];
		auto& pos = view.get<PositionComponent>(entity);
		auto& rot = view.get<RotationComponent>(entity);
		auto& fDir = view.get<ForwardDirection>(entity);
		auto& neighs = view.get<Neighbours>(entity);
		auto& nDir = view.get<NeiDirection>(entity);
		auto& speed = view.get<FishSpeed>(entity);
		speed.speed = fishSpeed;
		float fishSep = fishSeperation;
		float fishCush = fishCushion;
		float fishSteer = fishSteerSpeed;

		//for (auto [entity, pos, rot, fDir, neighs, nDir, speed] : view.each()) {
		Vector2 newDirection = fDir.dir;

		if (neighs.pos.size() > 0) {
			float dist = 0, minDist = std::numeric_limits<float>::max();
			Vector2 tempPos{ 0,0 }, tempDir{ 0,0 }, medianPoint{ 0,0 }, medianDir{ 0,0 }, closest{ 0,0 };

			for (size_t i = 0; i < neighs.pos.size(); i++) {
				Vector2 otherPos = neighs.pos[i];
				tempPos += otherPos;

				float dist = Vector2DistanceSqr(pos.pos, otherPos);
				if (dist < minDist) {
					minDist = dist;
					closest = otherPos;
				}
			}

			medianPoint = { tempPos.x / neighs.pos.size(), tempPos.y / neighs.pos.size() };

			for (Vector2 otherDir : nDir.dir) tempDir += otherDir;
			medianDir = Vector2Normalize(tempDir);


			if (minDist < fishSep * fishSep) newDirection = GetDirectionOpposite(pos.pos, closest);
			else if (minDist > (fishSep + fishCush) * (fishSep + fishCush)) newDirection = GetDirectionTo(pos.pos, medianPoint);
			else newDirection = medianDir;
		}


		Vector2 borderAvoid{ 0.0f, 0.0f };
		const float borderForce = 50.0f;

		if (pos.pos.x < borderSize) {
			borderAvoid += {1.f, 0.f};
			pos.pos.x += borderForce * deltaTime;
		}
		if (pos.pos.x > mapSize.x - borderSize) {
			borderAvoid += {-1.f, 0.f};
			pos.pos.x -= borderForce * deltaTime;
		}
		if (pos.pos.y < borderSize) {
			borderAvoid += {0.f, 1.f};
			pos.pos.y += borderForce * deltaTime;
		}
		if (pos.pos.y > mapSize.y - borderSize) {
			borderAvoid += {0.f, -1.f};
			pos.pos.y -= borderForce * deltaTime;
		}

		if (borderAvoid.x != 0.f || borderAvoid.y != 0.f) {
			newDirection = Vector2Normalize(borderAvoid);
		}


		if (pos.pos.x < -200) fDir.dir = { 1,0 };
		else if (pos.pos.x > mapSize.x + 200) fDir.dir = { -1,0 };
		else if (pos.pos.y < -200) fDir.dir = { 0,1 };
		else if (pos.pos.y > mapSize.y + 200) fDir.dir = { 0,-1 };

		if (Vector2DistanceSqr(pos.pos, fishAvoidance) < 400 * 400) newDirection = GetDirectionOpposite(pos.pos, fishAvoidance);

		float steerSpeed = fishSteer * deltaTime;
		fDir.dir = Vector2Normalize(
			fDir.dir + (newDirection - fDir.dir) * steerSpeed
		);

		pos.pos += fDir.dir * speed.speed * deltaTime;

		float angleDeg = float(atan2(fDir.dir.y, fDir.dir.x) * (180.0f / PI));
		rot.angle = angleDeg + 90.0f;

	}
}

void Simulation::UpdateKoiChain(float deltaTime)
{
}

bool Simulation::LoadTextures()
{
	TraceLog(LOG_INFO, "Loading textures from memory...");

	// Load fish
	Image fishImg = LoadImageFromMemory(".png", SmallFish_png, SmallFish_png_len);
	if (fishImg.data == NULL) {
		TraceLog(LOG_ERROR, "Failed to load fish image from memory!");
		return false;
	}
	fishTexture = LoadTextureFromImage(fishImg);
	UnloadImage(fishImg);

	if (fishTexture.id == 0) {
		TraceLog(LOG_ERROR, "Failed to create fish texture!");
		return false;
	}
	TraceLog(LOG_INFO, "Fish texture loaded: %dx%d",
		fishTexture.width, fishTexture.height);
	return true;
}

Simulation::Simulation()
{
	camera.target = { mapSize.x / 2.0f, mapSize.y / 2.0f };
	camera.offset = { windowWidth / 2.0f, windowHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	cols = (int)mapSize.x / cellSize;
	rows = (int)mapSize.y / cellSize;
}
void Simulation::Start()
{
	if (!LoadTextures())
	{
		printf("ERROR: Failed to load textures\n");
	}

	spatialGrid.resize(cols * rows);
	for (auto& cell : spatialGrid) cell.reserve(64); // reserve some space to avoid multiple allocations


	for (unsigned int i = 0; i < fishCount; i++)
	{
		auto entity = registry.create();

		Vector2 pos{ static_cast<float>(GetRandomValue(0, (int)mapSize.x)),
			static_cast<float>(GetRandomValue(0, (int)mapSize.y)) };


		float angle = static_cast<float>(GetRandomValue(0, 360));
		float rad = (angle - 90.0f) * DEG2RAD;		
		Vector2 dir = { cosf(rad), sinf(rad) };

		PositionComponent posComp;
		posComp.pos = pos;

		RotationComponent rotComp;
		rotComp.angle = angle;

		ForwardDirection fDirComp;
		fDirComp.dir = dir;

		float speedComp = fishSpeed;

		registry.emplace<PositionComponent>(entity, posComp);
		registry.emplace<RotationComponent>(entity, rotComp);
		registry.emplace<ForwardDirection>(entity, fDirComp);
		registry.emplace<CellID>(entity, 0);
		registry.emplace<Neighbours>(entity);
		registry.emplace<NeiDirection>(entity);
		registry.emplace<FishSpeed>(entity, speedComp);
		registry.emplace<TagFish>(entity);
	}

	auto view = registry.view<Neighbours, NeiDirection>();
	for (auto [entity, neighs, nDir] : view.each()) {
		neighs.pos.clear();
		neighs.pos.reserve(64);
		nDir.dir.clear();
		nDir.dir.reserve(64);
	}

}
void Simulation::Update(float deltaTime)
{
	UpdateSpatialGrid();
	FindNeighbors();
	UpdateFishMovement(deltaTime);
	UpdateKoiChain(deltaTime);

	camera.zoom += (float)GetMouseWheelMove() * 0.05f;
	if (camera.zoom > 3.0f) camera.zoom = 3.0f;
	else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
		dragStart = GetScreenToWorld2D(GetMousePosition(), camera);
		isDragging = true;
	}
	if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) isDragging = false;

	if (isDragging) {
		Vector2 current = GetScreenToWorld2D(GetMousePosition(), camera);
		Vector2 delta = { dragStart.x - current.x, dragStart.y - current.y };//Vector2Subtract(dragStart, current);
		camera.target = { camera.target.x + delta.x, camera.target.y + delta.y };//Vector2Add(camera.target, delta);
	}

	//fishAvoidance = GetScreenToWorld2D(GetMousePosition(), camera);

}
void Simulation::Render()
{
	BeginMode2D(camera);
	// vertical lines
	for (float x = 0; x <= mapSize.x; x += cellSize)
		DrawLine((int)x, 0, (int)x, (int)mapSize.y, DARKGRAY);

	// horizontal lines
	for (float y = 0; y <= mapSize.y; y += cellSize)
		DrawLine(0, (int)y, (int)mapSize.x, (int)y, DARKGRAY);

	// optional map border
	DrawRectangleLines(0, 0, (int)mapSize.x, (int)mapSize.y, RED);


	auto view = registry.view<PositionComponent, RotationComponent, TagFish>();
	view.each([this](auto entity, PositionComponent& pos, RotationComponent& rot) {
		Rectangle sourceRec = { 0.0f, 0.0f, 32, 64 };
		Rectangle destRec = { pos.pos.x, pos.pos.y, 32, 64 };
		Vector2 origin = { 16, 32};
		DrawTexturePro(fishTexture, sourceRec, destRec, origin, rot.angle, WHITE);	
		
	});



	EndMode2D();

	DrawText(TextFormat("Fish count: %d", fishCount), 10, 10, 20, WHITE);
	if (GuiButton({ 10, 40, 100, 30 }, "Back to Main Menu")) {
		registry.clear();
		currentGameState = GameState::MENU;
	}

	

	GuiSlider({ float(windowWidth) - 200, 10, 150, 20}, "Fish Speed", TextFormat("%.1f", fishSpeed), &fishSpeed, 50.0f, 500.0f);
	GuiSlider({ float(windowWidth) - 200, 40, 150, 20 }, "Fish Coherence", TextFormat("%.1f", fishCoherence), &fishCoherence, 50.0f, 500.0f);
	GuiSlider({ float(windowWidth) - 200, 70, 150, 20 }, "Fish Separation", TextFormat("%.1f", fishSeperation), &fishSeperation, 5.0f, 100.0f);
	GuiSlider({ float(windowWidth) - 200, 100, 150, 20 }, "Fish Cushion", TextFormat("%.1f", fishCushion), &fishCushion, 5.0f, 100.0f);
	GuiSlider({ float(windowWidth) - 200, 130, 150, 20 }, "Fish Steer Speed", TextFormat("%.1f", fishSteerSpeed), &fishSteerSpeed, 1.0f, 20.0f);

	if (GuiButton({ float(windowWidth) - 200, 170, 150, 30 }, "Reset Settings")) {
		fishSpeed = 200.0f;
		fishCoherence = 200.0f;
		fishSeperation = 20.0f;
		fishCushion = 60.0f;
		fishSteerSpeed = 6.0f;
	}
	
}
