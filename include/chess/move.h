#ifndef MOVE_H
#define MOVE_H

#include "chess/board.h"
#include "chess/piece.h"

#define MOVE_LIST_MAX 256
#define MOVE_UCI_MAX 8


typedef enum MoveType
{
    moveTypeNormal = 0,
    moveTypeCapture,
    moveTypeDoublePawnPush,
    moveTypeEnPassant,
    moveTypeKingSideCastle,
    moveTypeQueenSideCastle,
    moveTypePromotion,
    moveTypePromotionCapture

} MoveType;


typedef struct Move
{
    BoardSquare from;
    BoardSquare to;
    MoveType type;
    PieceType promotionType;

} Move;


typedef struct MoveList
{
    Move moves[MOVE_LIST_MAX];
    int count;

} MoveList;


// Lifecycle
Move move_make(BoardSquare, BoardSquare, MoveType);
Move move_makePromotion(BoardSquare, BoardSquare, MoveType, PieceType);


// List
void moveList_clear(MoveList*);
int moveList_add(MoveList*, Move);
int moveList_containsToSquare(const MoveList*, BoardSquare);
const Move* moveList_findBySquares(const MoveList*, BoardSquare, BoardSquare);


// Utility
void move_writeUci(Move, char*, int);
const char* moveType_toString(MoveType);

#endif // MOVE_H