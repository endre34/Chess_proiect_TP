#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include "chess/game.h"
#include "game_session/game_clock.h"

#define GAME_SESSION_MOVE_MAX 512
#define GAME_SESSION_POSITION_MAX 512
#define GAME_SESSION_STOCKFISH_COMMAND_MAX 4096


typedef enum GameSessionMode
{
    gameSessionModeLocalPvp = 0,
    gameSessionModeEnginePve

} GameSessionMode;


typedef enum GameSessionStatus
{
    gameSessionStatusActive = 0,
    gameSessionStatusWhiteWon,
    gameSessionStatusBlackWon,
    gameSessionStatusDraw

} GameSessionStatus;


typedef enum GameSessionEndReason
{
    gameSessionEndReasonNone = 0,
    gameSessionEndReasonCheckmate,
    gameSessionEndReasonStalemate,
    gameSessionEndReasonInsufficientMaterial,
    gameSessionEndReasonThreefoldRepetition,
    gameSessionEndReasonTimeout

} GameSessionEndReason;


typedef struct GameSessionMoveRecord
{
    Move move;
    char uci[MOVE_UCI_MAX];
    char positionKey[GAME_POSITION_KEY_MAX];

} GameSessionMoveRecord;


typedef struct GameSession GameSession;


// Lifecycle
GameSession* gameSession_create(GameSessionMode, PieceColor, int, int);
void gameSession_destroy(GameSession*);
void gameSession_reset(GameSession*);


// State
const Game* gameSession_getGame(const GameSession*);
const GameClock* gameSession_getClock(const GameSession*);

GameSessionMode gameSession_getMode(const GameSession*);
GameSessionStatus gameSession_getStatus(const GameSession*);
GameSessionEndReason gameSession_getEndReason(const GameSession*);
PieceColor gameSession_getWinner(const GameSession*);
PieceColor gameSession_getHumanColor(const GameSession*);

int gameSession_getEngineLevel(const GameSession*);
void gameSession_setEngineLevel(GameSession*, int);


// View
BoardPerspective gameSession_getPerspective(const GameSession*);


// Time
void gameSession_update(GameSession*, int);
void gameSession_pauseClock(GameSession*);
void gameSession_resumeClock(GameSession*);
int gameSession_getRemainingMilliseconds(const GameSession*, PieceColor);
void gameSession_writeTime(const GameSession*, PieceColor, char*, int);


// Moves
int gameSession_makeMove(GameSession*, BoardSquare, BoardSquare);
int gameSession_getMoveCount(const GameSession*);
const GameSessionMoveRecord* gameSession_getMoveRecord(const GameSession*, int);


// Stockfish
void gameSession_writeStockfishPositionCommand(const GameSession*, char*, int);


// Utility
const char* gameSessionStatus_toString(GameSessionStatus);
const char* gameSessionEndReason_toString(GameSessionEndReason);

#endif // GAME_SESSION_H