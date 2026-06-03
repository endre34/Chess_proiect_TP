#include "chess/move.h"

#include <stdio.h>


static char promotionChar(PieceType type)
{
    if (type == pieceTypeKnight)
    {
        return 'n';
    }

    if (type == pieceTypeBishop)
    {
        return 'b';
    }

    if (type == pieceTypeRook)
    {
        return 'r';
    }

    return 'q';
}


Move move_make(BoardSquare from, BoardSquare to, MoveType type)
{
    Move move;

    move.from = from;
    move.to = to;
    move.type = type;
    move.promotionType = pieceTypeNone;

    return move;
}


Move move_makePromotion(BoardSquare from, BoardSquare to, MoveType type, PieceType promotionType)
{
    Move move;

    move = move_make(from, to, type);
    move.promotionType = promotionType;

    return move;
}


void moveList_clear(MoveList* list)
{
    if (list == NULL)
    {
        return;
    }

    list->count = 0;
}


int moveList_add(MoveList* list, Move move)
{
    if (list == NULL)
    {
        return 0;
    }

    if (list->count >= MOVE_LIST_MAX)
    {
        return 0;
    }

    list->moves[list->count] = move;
    list->count++;

    return 1;
}


int moveList_containsToSquare(const MoveList* list, BoardSquare to)
{
    int i;

    if (list == NULL)
    {
        return 0;
    }

    for (i = 0; i < list->count; i++)
    {
        if (board_squaresAreEqual(list->moves[i].to, to))
        {
            return 1;
        }
    }

    return 0;
}


const Move* moveList_findBySquares(const MoveList* list, BoardSquare from, BoardSquare to)
{
    int i;

    if (list == NULL)
    {
        return NULL;
    }

    for (i = 0; i < list->count; i++)
    {
        if (board_squaresAreEqual(list->moves[i].from, from) &&
            board_squaresAreEqual(list->moves[i].to, to))
        {
            return &list->moves[i];
        }
    }

    return NULL;
}


void move_writeUci(Move move, char* buffer, int bufferSize)
{
    char fromFile;
    char fromRank;
    char toFile;
    char toRank;

    if (buffer == NULL || bufferSize <= 0)
    {
        return;
    }

    buffer[0] = '\0';

    if (!board_isValidSquare(move.from) || !board_isValidSquare(move.to))
    {
        return;
    }

    fromFile = (char)('a' + move.from.file);
    fromRank = (char)('1' + move.from.rank);
    toFile = (char)('a' + move.to.file);
    toRank = (char)('1' + move.to.rank);

    if (move.type == moveTypePromotion || move.type == moveTypePromotionCapture)
    {
        snprintf(buffer, bufferSize, "%c%c%c%c%c", fromFile, fromRank, toFile, toRank, promotionChar(move.promotionType));
        return;
    }

    snprintf(buffer, bufferSize, "%c%c%c%c", fromFile, fromRank, toFile, toRank);
}


const char* moveType_toString(MoveType type)
{
    switch (type)
    {
        case moveTypeNormal:
            return "Normal";

        case moveTypeCapture:
            return "Capture";

        case moveTypeDoublePawnPush:
            return "Double pawn push";

        case moveTypeEnPassant:
            return "En passant";

        case moveTypeKingSideCastle:
            return "King-side castle";

        case moveTypeQueenSideCastle:
            return "Queen-side castle";

        case moveTypePromotion:
            return "Promotion";

        case moveTypePromotionCapture:
            return "Promotion capture";

        default:
            return "Invalid";
    }
}