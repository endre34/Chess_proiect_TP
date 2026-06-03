#ifndef GAME_H
#define GAME_H

#include "chess/board.h"
#include "chess/move.h"
#include "chess/piece.h"

#define GAME_POSITION_KEY_MAX 128


typedef enum GameStatus
{
    gameStatusActive = 0,
    gameStatusWhiteCheckmate,
    gameStatusBlackCheckmate,
    gameStatusStalemate,
    gameStatusInsufficientMaterial

} GameStatus;


typedef enum CastlingRight
{
    castlingRightNone = 0,
    castlingRightWhiteKingSide = 1 << 0,
    castlingRightWhiteQueenSide = 1 << 1,
    castlingRightBlackKingSide = 1 << 2,
    castlingRightBlackQueenSide = 1 << 3

} CastlingRight;


typedef struct Game Game;


// Lifecycle
Game* game_create(void);
Game* game_copy(const Game*);
void game_destroy(Game*);


// State
void game_reset(Game*);

const Board* game_getBoard(const Game*);
PieceColor game_getSideToMove(const Game*);
GameStatus game_getStatus(const Game*);
PieceColor game_getWinner(const Game*);


// Special state
int game_hasEnPassantTarget(const Game*);
BoardSquare game_getEnPassantTarget(const Game*);
int game_hasCastlingRight(const Game*, CastlingRight);


// Check
int game_isSquareAttacked(const Game*, BoardSquare, PieceColor);
int game_isInCheck(const Game*, PieceColor);


// Draws
int game_isInsufficientMaterial(const Game*);


// Moves
void game_getLegalMoves(const Game*, MoveList*);
void game_getLegalMovesFrom(const Game*, BoardSquare, MoveList*);

int game_isLegalMove(const Game*, BoardSquare, BoardSquare);
int game_makeMove(Game*, BoardSquare, BoardSquare);
int game_makeMoveRecord(Game*, BoardSquare, BoardSquare, Move*);


// Utility
void game_writePositionKey(const Game*, char*, int);
const char* gameStatus_toString(GameStatus);

#endif // GAME_H