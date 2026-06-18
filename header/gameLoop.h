#ifndef GAME_LOOP_H
#define GAME_LOOP_H

extern int screenWidth;
extern int screenHeight;
extern float delta;

void InitGameLoop(void);
void UpdateGameLoop(void);
void DrawGameLoop(void);
void CloseGameLoop(void);

#endif
