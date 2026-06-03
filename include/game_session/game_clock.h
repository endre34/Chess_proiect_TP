#ifndef GAME_CLOCK_H
#define GAME_CLOCK_H

#include "chess/piece.h"

#define GAME_CLOCK_TEXT_MAX 32


typedef enum GameClockStatus
{
    gameClockStatusStopped = 0,
    gameClockStatusRunning,
    gameClockStatusPaused,
    gameClockStatusExpired

} GameClockStatus;


typedef struct GameClock GameClock;


// Lifecycle
GameClock* gameClock_create(int, int);
void gameClock_destroy(GameClock*);
void gameClock_configure(GameClock*, int, int);
void gameClock_reset(GameClock*);


// State
int gameClock_hasTimeControl(const GameClock*);
GameClockStatus gameClock_getStatus(const GameClock*);
PieceColor gameClock_getActiveColor(const GameClock*);
PieceColor gameClock_getExpiredColor(const GameClock*);

int gameClock_getInitialMilliseconds(const GameClock*);
int gameClock_getIncrementMilliseconds(const GameClock*);
int gameClock_getRemainingMilliseconds(const GameClock*, PieceColor);


// Control
void gameClock_start(GameClock*, PieceColor);
void gameClock_pause(GameClock*);
void gameClock_resume(GameClock*);
void gameClock_stop(GameClock*);
void gameClock_setActiveColor(GameClock*, PieceColor);


// Update
void gameClock_update(GameClock*, int);
void gameClock_finishTurn(GameClock*, PieceColor);


// Utility
void gameClock_writeTime(const GameClock*, PieceColor, char*, int);
const char* gameClockStatus_toString(GameClockStatus);

#endif // GAME_CLOCK_H