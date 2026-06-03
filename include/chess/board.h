#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>

#include "chess/piece.h"

#define BOARD_SIZE 8
#define BOARD_SQUARE_COUNT 64


typedef enum BoardPerspective
{
    boardPerspectiveWhite = 0,
    boardPerspectiveBlack

} BoardPerspective;


typedef struct BoardSquare
{
    int file;
    int rank;

} BoardSquare;


typedef struct BoardViewSquare
{
    int column;
    int row;

} BoardViewSquare;


typedef struct Board Board;


// Lifecycle
Board* board_create(void);
Board* board_copy(const Board*);
void board_destroy(Board*);


// State
void board_clear(Board*);
void board_setStartingPosition(Board*);


// Squares
BoardSquare board_makeSquare(int, int);
BoardSquare board_invalidSquare(void);
BoardSquare board_squareFromAlgebraic(char, char);

int board_isValidSquare(BoardSquare);
int board_squaresAreEqual(BoardSquare, BoardSquare);
int board_squareColor(BoardSquare);


// View
BoardViewSquare board_makeViewSquare(int, int);
int board_isValidViewSquare(BoardViewSquare);

BoardSquare board_viewSquareToBoardSquare(BoardViewSquare, BoardPerspective);
BoardViewSquare board_boardSquareToViewSquare(BoardSquare, BoardPerspective);
BoardPerspective board_perspectiveFromPieceColor(PieceColor);


// Pieces
Piece board_getPiece(const Board*, BoardSquare);
void board_setPiece(Board*, BoardSquare, Piece);
Piece board_removePiece(Board*, BoardSquare);
int board_isEmptySquare(const Board*, BoardSquare);


// Movement
int board_movePiece(Board*, BoardSquare, BoardSquare);


// Debug
void board_print(const Board*, FILE*);
void board_printPerspective(const Board*, FILE*, BoardPerspective);

#endif // BOARD_H