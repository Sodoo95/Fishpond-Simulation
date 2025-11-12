#ifndef GLOBALS_H
#define GLOBALS_H
enum class GameState
{
	MENU,
	SIMULATION,
	EXIT
};
inline GameState currentGameState = GameState::MENU;
inline bool isRunning = true;
inline bool isPaused = false;
inline bool isDebugMode = false;
inline unsigned short windowWidth = 1366;
inline unsigned short windowHeight = 768;
inline int fishCount = 20000;

#endif // !GLOBALS_H


