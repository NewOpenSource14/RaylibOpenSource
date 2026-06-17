#pragma once
extern enum GameScreen { VICTORY, GAMEPLAY, ENDING } GameScreen;

void gameLogic(enum GameScreen *currentScreen);
