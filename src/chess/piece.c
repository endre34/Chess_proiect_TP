#include "chess/piece.h"


static int char_isUpper(char c)
{
    return c >= 'A' && c <= 'Z';
}


static char char_toLower(char c)
{
    if (char_isUpper(c))
    {
        return (char)(c - 'A' + 'a');
    }

    return c;
}


Piece piece_make(PieceType type, PieceColor color)
{
    Piece piece;

    piece.type = type;
    piece.color = color;

    if (!piece_isValid(piece))
    {
        return piece_empty();
    }

    return piece;
}


Piece piece_empty(void)
{
    Piece piece;

    piece.type = pieceTypeNone;
    piece.color = pieceColorNone;

    return piece;
}


int piece_isEmpty(Piece piece)
{
    return piece.type == pieceTypeNone;
}


int piece_isWhite(Piece piece)
{
    return piece.color == pieceColorWhite && !piece_isEmpty(piece);
}


int piece_isBlack(Piece piece)
{
    return piece.color == pieceColorBlack && !piece_isEmpty(piece);
}


int piece_isValid(Piece piece)
{
    if (piece.type < pieceTypeNone || piece.type > pieceTypeKing)
    {
        return 0;
    }

    if (piece.color < pieceColorNone || piece.color > pieceColorBlack)
    {
        return 0;
    }

    if (piece.type == pieceTypeNone)
    {
        return piece.color == pieceColorNone;
    }

    return piece.color == pieceColorWhite || piece.color == pieceColorBlack;
}


PieceColor piece_oppositeColor(PieceColor color)
{
    if (color == pieceColorWhite)
    {
        return pieceColorBlack;
    }

    if (color == pieceColorBlack)
    {
        return pieceColorWhite;
    }

    return pieceColorNone;
}


char piece_toFenChar(Piece piece)
{
    char c;

    if (!piece_isValid(piece) || piece_isEmpty(piece))
    {
        return '.';
    }

    switch (piece.type)
    {
        case pieceTypePawn:
            c = 'p';
            break;

        case pieceTypeKnight:
            c = 'n';
            break;

        case pieceTypeBishop:
            c = 'b';
            break;

        case pieceTypeRook:
            c = 'r';
            break;

        case pieceTypeQueen:
            c = 'q';
            break;

        case pieceTypeKing:
            c = 'k';
            break;

        default:
            return '.';
    }

    if (piece.color == pieceColorWhite)
    {
        c = (char)(c - 'a' + 'A');
    }

    return c;
}


Piece piece_fromFenChar(char c)
{
    PieceColor color;
    PieceType type;

    color = char_isUpper(c) ? pieceColorWhite : pieceColorBlack;

    switch (char_toLower(c))
    {
        case 'p':
            type = pieceTypePawn;
            break;

        case 'n':
            type = pieceTypeKnight;
            break;

        case 'b':
            type = pieceTypeBishop;
            break;

        case 'r':
            type = pieceTypeRook;
            break;

        case 'q':
            type = pieceTypeQueen;
            break;

        case 'k':
            type = pieceTypeKing;
            break;

        default:
            return piece_empty();
    }

    return piece_make(type, color);
}


const char* pieceType_toString(PieceType type)
{
    switch (type)
    {
        case pieceTypeNone:
            return "None";

        case pieceTypePawn:
            return "Pawn";

        case pieceTypeKnight:
            return "Knight";

        case pieceTypeBishop:
            return "Bishop";

        case pieceTypeRook:
            return "Rook";

        case pieceTypeQueen:
            return "Queen";

        case pieceTypeKing:
            return "King";

        default:
            return "Invalid";
    }
}


const char* pieceColor_toString(PieceColor color)
{
    switch (color)
    {
        case pieceColorNone:
            return "None";

        case pieceColorWhite:
            return "White";

        case pieceColorBlack:
            return "Black";

        default:
            return "Invalid";
    }
}