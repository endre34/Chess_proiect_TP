#ifndef PIECE_H
#define PIECE_H

typedef enum PieceType
{
    pieceTypeNone = 0,
    pieceTypePawn,
    pieceTypeKnight,
    pieceTypeBishop,
    pieceTypeRook,
    pieceTypeQueen,
    pieceTypeKing

} PieceType;


typedef enum PieceColor
{
    pieceColorNone = 0,
    pieceColorWhite,
    pieceColorBlack

} PieceColor;


typedef struct Piece
{
    PieceType type;
    PieceColor color;

} Piece;


// Lifecycle
Piece piece_make(PieceType, PieceColor);
Piece piece_empty(void);


// State
int piece_isEmpty(Piece);
int piece_isWhite(Piece);
int piece_isBlack(Piece);
int piece_isValid(Piece);


// Utility
PieceColor piece_oppositeColor(PieceColor);

char piece_toFenChar(Piece);
Piece piece_fromFenChar(char);

const char* pieceType_toString(PieceType);
const char* pieceColor_toString(PieceColor);

#endif // PIECE_H