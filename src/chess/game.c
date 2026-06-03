#include "chess/game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Game
{
    Board* board;
    PieceColor sideToMove;
    int castlingRights;
    BoardSquare enPassantTarget;
    int hasEnPassantTarget;
    GameStatus status;
    PieceColor winner;
};


// Utility
static int pawnDirection(PieceColor color)
{
    if (color == pieceColorWhite)
    {
        return 1;
    }

    if (color == pieceColorBlack)
    {
        return -1;
    }

    return 0;
}


static int pawnStartRank(PieceColor color)
{
    if (color == pieceColorWhite)
    {
        return 1;
    }

    if (color == pieceColorBlack)
    {
        return 6;
    }

    return -1;
}


static int pawnPromotionRank(PieceColor color)
{
    if (color == pieceColorWhite)
    {
        return 7;
    }

    if (color == pieceColorBlack)
    {
        return 0;
    }

    return -1;
}


static int isEnemyPiece(Piece piece, PieceColor color)
{
    return !piece_isEmpty(piece) && piece.color == piece_oppositeColor(color);
}


static int isOwnPiece(Piece piece, PieceColor color)
{
    return !piece_isEmpty(piece) && piece.color == color;
}


static int castlingRightFor(PieceColor color, int kingSide)
{
    if (color == pieceColorWhite && kingSide)
    {
        return castlingRightWhiteKingSide;
    }

    if (color == pieceColorWhite)
    {
        return castlingRightWhiteQueenSide;
    }

    if (color == pieceColorBlack && kingSide)
    {
        return castlingRightBlackKingSide;
    }

    if (color == pieceColorBlack)
    {
        return castlingRightBlackQueenSide;
    }

    return castlingRightNone;
}


static void appendText(char* buffer, int bufferSize, const char* text)
{
    int currentLength;
    int remaining;

    if (buffer == NULL || text == NULL || bufferSize <= 0)
    {
        return;
    }

    currentLength = (int)strlen(buffer);

    if (currentLength >= bufferSize - 1)
    {
        return;
    }

    remaining = bufferSize - currentLength;
    snprintf(buffer + currentLength, remaining, "%s", text);
}


// Attacks
static BoardSquare findKingOnBoard(const Board* board, PieceColor color)
{
    int rank;
    int file;

    for (rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (file = 0; file < BOARD_SIZE; file++)
        {
            BoardSquare square;
            Piece piece;

            square = board_makeSquare(file, rank);
            piece = board_getPiece(board, square);

            if (piece.type == pieceTypeKing && piece.color == color)
            {
                return square;
            }
        }
    }

    return board_invalidSquare();
}


static int isSquareAttackedByPawns(const Board* board, BoardSquare square, PieceColor color)
{
    int fileOffset;
    int sourceRank;

    sourceRank = square.rank - pawnDirection(color);

    for (fileOffset = -1; fileOffset <= 1; fileOffset += 2)
    {
        BoardSquare from;
        Piece piece;

        from = board_makeSquare(square.file + fileOffset, sourceRank);

        if (!board_isValidSquare(from))
        {
            continue;
        }

        piece = board_getPiece(board, from);

        if (piece.type == pieceTypePawn && piece.color == color)
        {
            return 1;
        }
    }

    return 0;
}


static int isSquareAttackedByKnights(const Board* board, BoardSquare square, PieceColor color)
{
    static const int offsets[8][2] =
    {
        { 1, 2 }, { 2, 1 }, { 2, -1 }, { 1, -2 },
        { -1, -2 }, { -2, -1 }, { -2, 1 }, { -1, 2 }
    };

    int i;

    for (i = 0; i < 8; i++)
    {
        BoardSquare from;
        Piece piece;

        from = board_makeSquare(square.file + offsets[i][0], square.rank + offsets[i][1]);

        if (!board_isValidSquare(from))
        {
            continue;
        }

        piece = board_getPiece(board, from);

        if (piece.type == pieceTypeKnight && piece.color == color)
        {
            return 1;
        }
    }

    return 0;
}


static int isSquareAttackedByKing(const Board* board, BoardSquare square, PieceColor color)
{
    int fileOffset;
    int rankOffset;

    for (rankOffset = -1; rankOffset <= 1; rankOffset++)
    {
        for (fileOffset = -1; fileOffset <= 1; fileOffset++)
        {
            BoardSquare from;
            Piece piece;

            if (fileOffset == 0 && rankOffset == 0)
            {
                continue;
            }

            from = board_makeSquare(square.file + fileOffset, square.rank + rankOffset);

            if (!board_isValidSquare(from))
            {
                continue;
            }

            piece = board_getPiece(board, from);

            if (piece.type == pieceTypeKing && piece.color == color)
            {
                return 1;
            }
        }
    }

    return 0;
}


static int isSquareAttackedBySliders(const Board* board, BoardSquare square, PieceColor color)
{
    static const int directions[8][2] =
    {
        { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 },
        { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 }
    };

    int i;

    for (i = 0; i < 8; i++)
    {
        BoardSquare current;

        current = board_makeSquare(square.file + directions[i][0], square.rank + directions[i][1]);

        while (board_isValidSquare(current))
        {
            Piece piece;
            int diagonal;

            piece = board_getPiece(board, current);
            diagonal = directions[i][0] != 0 && directions[i][1] != 0;

            if (!piece_isEmpty(piece))
            {
                if (piece.color == color && piece.type == pieceTypeQueen)
                {
                    return 1;
                }

                if (piece.color == color && diagonal && piece.type == pieceTypeBishop)
                {
                    return 1;
                }

                if (piece.color == color && !diagonal && piece.type == pieceTypeRook)
                {
                    return 1;
                }

                break;
            }

            current.file += directions[i][0];
            current.rank += directions[i][1];
        }
    }

    return 0;
}


static int isSquareAttackedOnBoard(const Board* board, BoardSquare square, PieceColor color)
{
    if (board == NULL || !board_isValidSquare(square))
    {
        return 0;
    }

    if (color != pieceColorWhite && color != pieceColorBlack)
    {
        return 0;
    }

    if (isSquareAttackedByPawns(board, square, color))
    {
        return 1;
    }

    if (isSquareAttackedByKnights(board, square, color))
    {
        return 1;
    }

    if (isSquareAttackedByKing(board, square, color))
    {
        return 1;
    }

    return isSquareAttackedBySliders(board, square, color);
}


static int isKingInCheckOnBoard(const Board* board, PieceColor color)
{
    BoardSquare kingSquare;

    kingSquare = findKingOnBoard(board, color);

    if (!board_isValidSquare(kingSquare))
    {
        return 0;
    }

    return isSquareAttackedOnBoard(board, kingSquare, piece_oppositeColor(color));
}


// Draws
static int isInsufficientMaterialOnBoard(const Board* board)
{
    int rank;
    int file;
    int minorCount;
    int bishopCount;
    int knightCount;
    int bishopSquareColor;
    int sameBishopColor;

    minorCount = 0;
    bishopCount = 0;
    knightCount = 0;
    bishopSquareColor = -1;
    sameBishopColor = 1;

    for (rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (file = 0; file < BOARD_SIZE; file++)
        {
            BoardSquare square;
            Piece piece;

            square = board_makeSquare(file, rank);
            piece = board_getPiece(board, square);

            if (piece_isEmpty(piece) || piece.type == pieceTypeKing)
            {
                continue;
            }

            if (piece.type == pieceTypePawn || piece.type == pieceTypeRook || piece.type == pieceTypeQueen)
            {
                return 0;
            }

            if (piece.type == pieceTypeBishop)
            {
                int color;

                color = board_squareColor(square);

                if (bishopSquareColor == -1)
                {
                    bishopSquareColor = color;
                }
                else if (bishopSquareColor != color)
                {
                    sameBishopColor = 0;
                }

                bishopCount++;
                minorCount++;
            }

            if (piece.type == pieceTypeKnight)
            {
                knightCount++;
                minorCount++;
            }
        }
    }

    if (minorCount == 0)
    {
        return 1;
    }

    if (minorCount == 1)
    {
        return 1;
    }

    if (knightCount == 0 && bishopCount > 0 && sameBishopColor)
    {
        return 1;
    }

    return 0;
}


// Move adding
static void addMoveOrPromotion(MoveList* moves, BoardSquare from, BoardSquare to, MoveType type, PieceColor color)
{
    MoveType promotionType;

    if (to.rank != pawnPromotionRank(color))
    {
        moveList_add(moves, move_make(from, to, type));
        return;
    }

    promotionType = type == moveTypeCapture ? moveTypePromotionCapture : moveTypePromotion;
    moveList_add(moves, move_makePromotion(from, to, promotionType, pieceTypeQueen));
}


static void addPawnMoves(const Game* game, BoardSquare from, MoveList* moves)
{
    Piece pawn;
    int direction;
    int fileOffset;
    BoardSquare oneForward;
    BoardSquare twoForward;

    pawn = board_getPiece(game->board, from);
    direction = pawnDirection(pawn.color);

    oneForward = board_makeSquare(from.file, from.rank + direction);

    if (board_isValidSquare(oneForward) && board_isEmptySquare(game->board, oneForward))
    {
        addMoveOrPromotion(moves, from, oneForward, moveTypeNormal, pawn.color);

        twoForward = board_makeSquare(from.file, from.rank + 2 * direction);

        if (from.rank == pawnStartRank(pawn.color) && board_isValidSquare(twoForward))
        {
            if (board_isEmptySquare(game->board, twoForward))
            {
                moveList_add(moves, move_make(from, twoForward, moveTypeDoublePawnPush));
            }
        }
    }

    for (fileOffset = -1; fileOffset <= 1; fileOffset += 2)
    {
        BoardSquare to;
        Piece target;

        to = board_makeSquare(from.file + fileOffset, from.rank + direction);

        if (!board_isValidSquare(to))
        {
            continue;
        }

        target = board_getPiece(game->board, to);

        if (isEnemyPiece(target, pawn.color))
        {
            addMoveOrPromotion(moves, from, to, moveTypeCapture, pawn.color);
        }

        if (game->hasEnPassantTarget && board_squaresAreEqual(to, game->enPassantTarget))
        {
            BoardSquare capturedSquare;
            Piece capturedPiece;

            capturedSquare = board_makeSquare(to.file, from.rank);
            capturedPiece = board_getPiece(game->board, capturedSquare);

            if (capturedPiece.type == pieceTypePawn)
            {
                if (capturedPiece.color == piece_oppositeColor(pawn.color))
                {
                    moveList_add(moves, move_make(from, to, moveTypeEnPassant));
                }
            }
        }
    }
}


static void addKnightMoves(const Game* game, BoardSquare from, MoveList* moves)
{
    static const int offsets[8][2] =
    {
        { 1, 2 }, { 2, 1 }, { 2, -1 }, { 1, -2 },
        { -1, -2 }, { -2, -1 }, { -2, 1 }, { -1, 2 }
    };

    Piece knight;
    int i;

    knight = board_getPiece(game->board, from);

    for (i = 0; i < 8; i++)
    {
        BoardSquare to;
        Piece target;

        to = board_makeSquare(from.file + offsets[i][0], from.rank + offsets[i][1]);

        if (!board_isValidSquare(to))
        {
            continue;
        }

        target = board_getPiece(game->board, to);

        if (isOwnPiece(target, knight.color))
        {
            continue;
        }

        if (piece_isEmpty(target))
        {
            moveList_add(moves, move_make(from, to, moveTypeNormal));
        }
        else
        {
            moveList_add(moves, move_make(from, to, moveTypeCapture));
        }
    }
}


static void addSlidingMoves(const Game* game, BoardSquare from, MoveList* moves, const int directions[][2], int count)
{
    Piece piece;
    int i;

    piece = board_getPiece(game->board, from);

    for (i = 0; i < count; i++)
    {
        BoardSquare to;

        to = board_makeSquare(from.file + directions[i][0], from.rank + directions[i][1]);

        while (board_isValidSquare(to))
        {
            Piece target;

            target = board_getPiece(game->board, to);

            if (isOwnPiece(target, piece.color))
            {
                break;
            }

            if (piece_isEmpty(target))
            {
                moveList_add(moves, move_make(from, to, moveTypeNormal));
            }
            else
            {
                moveList_add(moves, move_make(from, to, moveTypeCapture));
                break;
            }

            to.file += directions[i][0];
            to.rank += directions[i][1];
        }
    }
}


static void addBishopMoves(const Game* game, BoardSquare from, MoveList* moves)
{
    static const int directions[4][2] =
    {
        { 1, 1 }, { 1, -1 }, { -1, -1 }, { -1, 1 }
    };

    addSlidingMoves(game, from, moves, directions, 4);
}


static void addRookMoves(const Game* game, BoardSquare from, MoveList* moves)
{
    static const int directions[4][2] =
    {
        { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }
    };

    addSlidingMoves(game, from, moves, directions, 4);
}


static void addQueenMoves(const Game* game, BoardSquare from, MoveList* moves)
{
    static const int directions[8][2] =
    {
        { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 },
        { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 }
    };

    addSlidingMoves(game, from, moves, directions, 8);
}


// Castling
static int canCastleKingSide(const Game* game, PieceColor color)
{
    int rank;
    Piece king;
    Piece rook;
    PieceColor enemyColor;

    rank = color == pieceColorWhite ? 0 : 7;
    enemyColor = piece_oppositeColor(color);

    if (!(game->castlingRights & castlingRightFor(color, 1)))
    {
        return 0;
    }

    king = board_getPiece(game->board, board_makeSquare(4, rank));
    rook = board_getPiece(game->board, board_makeSquare(7, rank));

    if (king.type != pieceTypeKing || king.color != color)
    {
        return 0;
    }

    if (rook.type != pieceTypeRook || rook.color != color)
    {
        return 0;
    }

    if (!board_isEmptySquare(game->board, board_makeSquare(5, rank)))
    {
        return 0;
    }

    if (!board_isEmptySquare(game->board, board_makeSquare(6, rank)))
    {
        return 0;
    }

    if (isKingInCheckOnBoard(game->board, color))
    {
        return 0;
    }

    if (isSquareAttackedOnBoard(game->board, board_makeSquare(5, rank), enemyColor))
    {
        return 0;
    }

    return !isSquareAttackedOnBoard(game->board, board_makeSquare(6, rank), enemyColor);
}


static int canCastleQueenSide(const Game* game, PieceColor color)
{
    int rank;
    Piece king;
    Piece rook;
    PieceColor enemyColor;

    rank = color == pieceColorWhite ? 0 : 7;
    enemyColor = piece_oppositeColor(color);

    if (!(game->castlingRights & castlingRightFor(color, 0)))
    {
        return 0;
    }

    king = board_getPiece(game->board, board_makeSquare(4, rank));
    rook = board_getPiece(game->board, board_makeSquare(0, rank));

    if (king.type != pieceTypeKing || king.color != color)
    {
        return 0;
    }

    if (rook.type != pieceTypeRook || rook.color != color)
    {
        return 0;
    }

    if (!board_isEmptySquare(game->board, board_makeSquare(3, rank)))
    {
        return 0;
    }

    if (!board_isEmptySquare(game->board, board_makeSquare(2, rank)))
    {
        return 0;
    }

    if (!board_isEmptySquare(game->board, board_makeSquare(1, rank)))
    {
        return 0;
    }

    if (isKingInCheckOnBoard(game->board, color))
    {
        return 0;
    }

    if (isSquareAttackedOnBoard(game->board, board_makeSquare(3, rank), enemyColor))
    {
        return 0;
    }

    return !isSquareAttackedOnBoard(game->board, board_makeSquare(2, rank), enemyColor);
}


static void addKingMoves(const Game* game, BoardSquare from, MoveList* moves)
{
    Piece king;
    int fileOffset;
    int rankOffset;

    king = board_getPiece(game->board, from);

    for (rankOffset = -1; rankOffset <= 1; rankOffset++)
    {
        for (fileOffset = -1; fileOffset <= 1; fileOffset++)
        {
            BoardSquare to;
            Piece target;

            if (fileOffset == 0 && rankOffset == 0)
            {
                continue;
            }

            to = board_makeSquare(from.file + fileOffset, from.rank + rankOffset);

            if (!board_isValidSquare(to))
            {
                continue;
            }

            target = board_getPiece(game->board, to);

            if (isOwnPiece(target, king.color))
            {
                continue;
            }

            if (piece_isEmpty(target))
            {
                moveList_add(moves, move_make(from, to, moveTypeNormal));
            }
            else
            {
                moveList_add(moves, move_make(from, to, moveTypeCapture));
            }
        }
    }

    if (canCastleKingSide(game, king.color))
    {
        moveList_add(moves, move_make(from, board_makeSquare(6, from.rank), moveTypeKingSideCastle));
    }

    if (canCastleQueenSide(game, king.color))
    {
        moveList_add(moves, move_make(from, board_makeSquare(2, from.rank), moveTypeQueenSideCastle));
    }
}


// Pseudo moves
static void getPseudoMovesFrom(const Game* game, BoardSquare from, MoveList* moves)
{
    Piece piece;

    piece = board_getPiece(game->board, from);

    if (piece_isEmpty(piece))
    {
        return;
    }

    if (piece.color != game->sideToMove)
    {
        return;
    }

    switch (piece.type)
    {
        case pieceTypePawn:
            addPawnMoves(game, from, moves);
            break;

        case pieceTypeKnight:
            addKnightMoves(game, from, moves);
            break;

        case pieceTypeBishop:
            addBishopMoves(game, from, moves);
            break;

        case pieceTypeRook:
            addRookMoves(game, from, moves);
            break;

        case pieceTypeQueen:
            addQueenMoves(game, from, moves);
            break;

        case pieceTypeKing:
            addKingMoves(game, from, moves);
            break;

        default:
            break;
    }
}


static void getPseudoMoves(const Game* game, MoveList* moves)
{
    int rank;
    int file;

    moveList_clear(moves);

    if (game == NULL || moves == NULL)
    {
        return;
    }

    for (rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (file = 0; file < BOARD_SIZE; file++)
        {
            getPseudoMovesFrom(game, board_makeSquare(file, rank), moves);
        }
    }
}


// Rights
static void removeCastlingRightsForKing(Game* game, PieceColor color)
{
    if (color == pieceColorWhite)
    {
        game->castlingRights &= ~castlingRightWhiteKingSide;
        game->castlingRights &= ~castlingRightWhiteQueenSide;
    }

    if (color == pieceColorBlack)
    {
        game->castlingRights &= ~castlingRightBlackKingSide;
        game->castlingRights &= ~castlingRightBlackQueenSide;
    }
}


static void removeCastlingRightForRookSquare(Game* game, BoardSquare square)
{
    if (board_squaresAreEqual(square, board_makeSquare(0, 0)))
    {
        game->castlingRights &= ~castlingRightWhiteQueenSide;
    }

    if (board_squaresAreEqual(square, board_makeSquare(7, 0)))
    {
        game->castlingRights &= ~castlingRightWhiteKingSide;
    }

    if (board_squaresAreEqual(square, board_makeSquare(0, 7)))
    {
        game->castlingRights &= ~castlingRightBlackQueenSide;
    }

    if (board_squaresAreEqual(square, board_makeSquare(7, 7)))
    {
        game->castlingRights &= ~castlingRightBlackKingSide;
    }
}


// Applying
static void game_applyMoveUnchecked(Game* game, Move move)
{
    Piece movingPiece;
    Piece targetPiece;

    movingPiece = board_getPiece(game->board, move.from);
    targetPiece = board_getPiece(game->board, move.to);

    if (movingPiece.type == pieceTypeKing)
    {
        removeCastlingRightsForKing(game, movingPiece.color);
    }

    if (movingPiece.type == pieceTypeRook)
    {
        removeCastlingRightForRookSquare(game, move.from);
    }

    if (targetPiece.type == pieceTypeRook)
    {
        removeCastlingRightForRookSquare(game, move.to);
    }

    game->hasEnPassantTarget = 0;
    game->enPassantTarget = board_invalidSquare();

    if (move.type == moveTypeEnPassant)
    {
        BoardSquare capturedSquare;

        capturedSquare = board_makeSquare(move.to.file, move.from.rank);
        board_setPiece(game->board, capturedSquare, piece_empty());
    }

    board_setPiece(game->board, move.from, piece_empty());

    if (move.type == moveTypePromotion || move.type == moveTypePromotionCapture)
    {
        movingPiece = piece_make(pieceTypeQueen, movingPiece.color);
    }

    board_setPiece(game->board, move.to, movingPiece);

    if (move.type == moveTypeKingSideCastle)
    {
        int rank;
        Piece rook;

        rank = move.from.rank;
        rook = board_getPiece(game->board, board_makeSquare(7, rank));

        board_setPiece(game->board, board_makeSquare(7, rank), piece_empty());
        board_setPiece(game->board, board_makeSquare(5, rank), rook);
    }

    if (move.type == moveTypeQueenSideCastle)
    {
        int rank;
        Piece rook;

        rank = move.from.rank;
        rook = board_getPiece(game->board, board_makeSquare(0, rank));

        board_setPiece(game->board, board_makeSquare(0, rank), piece_empty());
        board_setPiece(game->board, board_makeSquare(3, rank), rook);
    }

    if (move.type == moveTypeDoublePawnPush)
    {
        game->hasEnPassantTarget = 1;
        game->enPassantTarget = board_makeSquare(move.from.file, (move.from.rank + move.to.rank) / 2);
    }

    game->sideToMove = piece_oppositeColor(game->sideToMove);
}


static void game_updateStatus(Game* game)
{
    MoveList moves;

    if (game == NULL)
    {
        return;
    }

    if (game_isInsufficientMaterial(game))
    {
        game->status = gameStatusInsufficientMaterial;
        game->winner = pieceColorNone;
        return;
    }

    game_getLegalMoves(game, &moves);

    if (moves.count > 0)
    {
        game->status = gameStatusActive;
        game->winner = pieceColorNone;
        return;
    }

    if (game_isInCheck(game, game->sideToMove))
    {
        if (game->sideToMove == pieceColorWhite)
        {
            game->status = gameStatusWhiteCheckmate;
            game->winner = pieceColorBlack;
        }
        else
        {
            game->status = gameStatusBlackCheckmate;
            game->winner = pieceColorWhite;
        }

        return;
    }

    game->status = gameStatusStalemate;
    game->winner = pieceColorNone;
}


// Lifecycle
Game* game_create(void)
{
    Game* game;

    game = malloc(sizeof(Game));

    if (game == NULL)
    {
        return NULL;
    }

    game->board = board_create();

    if (game->board == NULL)
    {
        free(game);
        return NULL;
    }

    game_reset(game);

    return game;
}


Game* game_copy(const Game* source)
{
    Game* copy;

    if (source == NULL)
    {
        return NULL;
    }

    copy = malloc(sizeof(Game));

    if (copy == NULL)
    {
        return NULL;
    }

    *copy = *source;
    copy->board = board_copy(source->board);

    if (copy->board == NULL)
    {
        free(copy);
        return NULL;
    }

    return copy;
}


void game_destroy(Game* game)
{
    if (game == NULL)
    {
        return;
    }

    board_destroy(game->board);
    free(game);
}


// State
void game_reset(Game* game)
{
    if (game == NULL)
    {
        return;
    }

    board_setStartingPosition(game->board);

    game->sideToMove = pieceColorWhite;
    game->castlingRights = castlingRightWhiteKingSide | castlingRightWhiteQueenSide |
                           castlingRightBlackKingSide | castlingRightBlackQueenSide;
    game->enPassantTarget = board_invalidSquare();
    game->hasEnPassantTarget = 0;
    game->status = gameStatusActive;
    game->winner = pieceColorNone;
}


const Board* game_getBoard(const Game* game)
{
    if (game == NULL)
    {
        return NULL;
    }

    return game->board;
}


PieceColor game_getSideToMove(const Game* game)
{
    if (game == NULL)
    {
        return pieceColorNone;
    }

    return game->sideToMove;
}


GameStatus game_getStatus(const Game* game)
{
    if (game == NULL)
    {
        return gameStatusActive;
    }

    return game->status;
}


PieceColor game_getWinner(const Game* game)
{
    if (game == NULL)
    {
        return pieceColorNone;
    }

    return game->winner;
}


// Special state
int game_hasEnPassantTarget(const Game* game)
{
    if (game == NULL)
    {
        return 0;
    }

    return game->hasEnPassantTarget;
}


BoardSquare game_getEnPassantTarget(const Game* game)
{
    if (game == NULL || !game->hasEnPassantTarget)
    {
        return board_invalidSquare();
    }

    return game->enPassantTarget;
}


int game_hasCastlingRight(const Game* game, CastlingRight right)
{
    if (game == NULL)
    {
        return 0;
    }

    return (game->castlingRights & right) != 0;
}


// Check
int game_isSquareAttacked(const Game* game, BoardSquare square, PieceColor color)
{
    if (game == NULL)
    {
        return 0;
    }

    return isSquareAttackedOnBoard(game->board, square, color);
}


int game_isInCheck(const Game* game, PieceColor color)
{
    if (game == NULL)
    {
        return 0;
    }

    return isKingInCheckOnBoard(game->board, color);
}


// Draws
int game_isInsufficientMaterial(const Game* game)
{
    if (game == NULL)
    {
        return 0;
    }

    return isInsufficientMaterialOnBoard(game->board);
}


// Moves
void game_getLegalMoves(const Game* game, MoveList* moves)
{
    MoveList pseudoMoves;
    int i;

    moveList_clear(moves);

    if (game == NULL || moves == NULL)
    {
        return;
    }

    if (game->status != gameStatusActive)
    {
        return;
    }

    getPseudoMoves(game, &pseudoMoves);

    for (i = 0; i < pseudoMoves.count; i++)
    {
        Game* copy;
        PieceColor movingColor;

        copy = game_copy(game);

        if (copy == NULL)
        {
            continue;
        }

        movingColor = game->sideToMove;
        game_applyMoveUnchecked(copy, pseudoMoves.moves[i]);

        if (!isKingInCheckOnBoard(copy->board, movingColor))
        {
            moveList_add(moves, pseudoMoves.moves[i]);
        }

        game_destroy(copy);
    }
}


void game_getLegalMovesFrom(const Game* game, BoardSquare from, MoveList* moves)
{
    MoveList allMoves;
    int i;

    moveList_clear(moves);

    if (game == NULL || moves == NULL)
    {
        return;
    }

    game_getLegalMoves(game, &allMoves);

    for (i = 0; i < allMoves.count; i++)
    {
        if (board_squaresAreEqual(allMoves.moves[i].from, from))
        {
            moveList_add(moves, allMoves.moves[i]);
        }
    }
}


int game_isLegalMove(const Game* game, BoardSquare from, BoardSquare to)
{
    MoveList moves;

    game_getLegalMovesFrom(game, from, &moves);

    return moveList_findBySquares(&moves, from, to) != NULL;
}


int game_makeMove(Game* game, BoardSquare from, BoardSquare to)
{
    return game_makeMoveRecord(game, from, to, NULL);
}


int game_makeMoveRecord(Game* game, BoardSquare from, BoardSquare to, Move* moveRecord)
{
    MoveList moves;
    const Move* move;

    if (game == NULL)
    {
        return 0;
    }

    game_getLegalMovesFrom(game, from, &moves);
    move = moveList_findBySquares(&moves, from, to);

    if (move == NULL)
    {
        return 0;
    }

    if (moveRecord != NULL)
    {
        *moveRecord = *move;
    }

    game_applyMoveUnchecked(game, *move);
    game_updateStatus(game);

    return 1;
}


// Utility
void game_writePositionKey(const Game* game, char* buffer, int bufferSize)
{
    int rank;
    int file;

    if (buffer == NULL || bufferSize <= 0)
    {
        return;
    }

    buffer[0] = '\0';

    if (game == NULL)
    {
        return;
    }

    for (rank = BOARD_SIZE - 1; rank >= 0; rank--)
    {
        for (file = 0; file < BOARD_SIZE; file++)
        {
            char pieceText[2];

            pieceText[0] = piece_toFenChar(board_getPiece(game->board, board_makeSquare(file, rank)));
            pieceText[1] = '\0';
            appendText(buffer, bufferSize, pieceText);
        }

        if (rank > 0)
        {
            appendText(buffer, bufferSize, "/");
        }
    }

    appendText(buffer, bufferSize, game->sideToMove == pieceColorWhite ? " w " : " b ");
    appendText(buffer, bufferSize, game->castlingRights & castlingRightWhiteKingSide ? "K" : "");
    appendText(buffer, bufferSize, game->castlingRights & castlingRightWhiteQueenSide ? "Q" : "");
    appendText(buffer, bufferSize, game->castlingRights & castlingRightBlackKingSide ? "k" : "");
    appendText(buffer, bufferSize, game->castlingRights & castlingRightBlackQueenSide ? "q" : "");

    if (game->castlingRights == castlingRightNone)
    {
        appendText(buffer, bufferSize, "-");
    }

    appendText(buffer, bufferSize, " ");

    if (game->hasEnPassantTarget)
    {
        char enPassantText[3];

        enPassantText[0] = (char)('a' + game->enPassantTarget.file);
        enPassantText[1] = (char)('1' + game->enPassantTarget.rank);
        enPassantText[2] = '\0';
        appendText(buffer, bufferSize, enPassantText);
    }
    else
    {
        appendText(buffer, bufferSize, "-");
    }
}


const char* gameStatus_toString(GameStatus status)
{
    switch (status)
    {
        case gameStatusActive:
            return "Active";

        case gameStatusWhiteCheckmate:
            return "White checkmated";

        case gameStatusBlackCheckmate:
            return "Black checkmated";

        case gameStatusStalemate:
            return "Stalemate";

        case gameStatusInsufficientMaterial:
            return "Insufficient material";

        default:
            return "Invalid";
    }
}