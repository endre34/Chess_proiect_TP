#include "chess/board.h"

#include <stdlib.h>


struct Board
{
    Piece squares[BOARD_SIZE][BOARD_SIZE];
};


static const PieceType startingBackRank[BOARD_SIZE] =
{
    pieceTypeRook,
    pieceTypeKnight,
    pieceTypeBishop,
    pieceTypeQueen,
    pieceTypeKing,
    pieceTypeBishop,
    pieceTypeKnight,
    pieceTypeRook
};


static void board_setBackRank(Board* board, int rank, PieceColor color)
{
    int file;

    for (file = 0; file < BOARD_SIZE; file++)
    {
        board->squares[rank][file] = piece_make(startingBackRank[file], color);
    }
}


static void board_setPawnRank(Board* board, int rank, PieceColor color)
{
    int file;

    for (file = 0; file < BOARD_SIZE; file++)
    {
        board->squares[rank][file] = piece_make(pieceTypePawn, color);
    }
}


Board* board_create(void)
{
    Board* board;

    board = malloc(sizeof(Board));

    if (board == NULL)
    {
        return NULL;
    }

    board_clear(board);

    return board;
}


Board* board_copy(const Board* source)
{
    Board* copy;

    if (source == NULL)
    {
        return NULL;
    }

    copy = malloc(sizeof(Board));

    if (copy == NULL)
    {
        return NULL;
    }

    *copy = *source;

    return copy;
}


void board_destroy(Board* board)
{
    free(board);
}


void board_clear(Board* board)
{
    int rank;
    int file;

    if (board == NULL)
    {
        return;
    }

    for (rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (file = 0; file < BOARD_SIZE; file++)
        {
            board->squares[rank][file] = piece_empty();
        }
    }
}


void board_setStartingPosition(Board* board)
{
    if (board == NULL)
    {
        return;
    }

    board_clear(board);

    board_setBackRank(board, 0, pieceColorWhite);
    board_setPawnRank(board, 1, pieceColorWhite);

    board_setPawnRank(board, 6, pieceColorBlack);
    board_setBackRank(board, 7, pieceColorBlack);
}


BoardSquare board_makeSquare(int file, int rank)
{
    BoardSquare square;

    square.file = file;
    square.rank = rank;

    return square;
}


BoardSquare board_invalidSquare(void)
{
    return board_makeSquare(-1, -1);
}


BoardSquare board_squareFromAlgebraic(char fileChar, char rankChar)
{
    return board_makeSquare(fileChar - 'a', rankChar - '1');
}


int board_isValidSquare(BoardSquare square)
{
    return square.file >= 0 && square.file < BOARD_SIZE &&
           square.rank >= 0 && square.rank < BOARD_SIZE;
}


int board_squaresAreEqual(BoardSquare left, BoardSquare right)
{
    return left.file == right.file && left.rank == right.rank;
}


int board_squareColor(BoardSquare square)
{
    if (!board_isValidSquare(square))
    {
        return -1;
    }

    return (square.file + square.rank) % 2;
}


BoardViewSquare board_makeViewSquare(int column, int row)
{
    BoardViewSquare square;

    square.column = column;
    square.row = row;

    return square;
}


int board_isValidViewSquare(BoardViewSquare square)
{
    return square.column >= 0 && square.column < BOARD_SIZE &&
           square.row >= 0 && square.row < BOARD_SIZE;
}


BoardSquare board_viewSquareToBoardSquare(BoardViewSquare square, BoardPerspective perspective)
{
    if (!board_isValidViewSquare(square))
    {
        return board_invalidSquare();
    }

    if (perspective == boardPerspectiveBlack)
    {
        return board_makeSquare(BOARD_SIZE - 1 - square.column, square.row);
    }

    return board_makeSquare(square.column, BOARD_SIZE - 1 - square.row);
}


BoardViewSquare board_boardSquareToViewSquare(BoardSquare square, BoardPerspective perspective)
{
    if (!board_isValidSquare(square))
    {
        return board_makeViewSquare(-1, -1);
    }

    if (perspective == boardPerspectiveBlack)
    {
        return board_makeViewSquare(BOARD_SIZE - 1 - square.file, square.rank);
    }

    return board_makeViewSquare(square.file, BOARD_SIZE - 1 - square.rank);
}


BoardPerspective board_perspectiveFromPieceColor(PieceColor color)
{
    if (color == pieceColorBlack)
    {
        return boardPerspectiveBlack;
    }

    return boardPerspectiveWhite;
}


Piece board_getPiece(const Board* board, BoardSquare square)
{
    if (board == NULL || !board_isValidSquare(square))
    {
        return piece_empty();
    }

    return board->squares[square.rank][square.file];
}


void board_setPiece(Board* board, BoardSquare square, Piece piece)
{
    if (board == NULL || !board_isValidSquare(square))
    {
        return;
    }

    if (!piece_isValid(piece))
    {
        return;
    }

    board->squares[square.rank][square.file] = piece;
}


Piece board_removePiece(Board* board, BoardSquare square)
{
    Piece removed;

    if (board == NULL || !board_isValidSquare(square))
    {
        return piece_empty();
    }

    removed = board->squares[square.rank][square.file];
    board->squares[square.rank][square.file] = piece_empty();

    return removed;
}


int board_isEmptySquare(const Board* board, BoardSquare square)
{
    return piece_isEmpty(board_getPiece(board, square));
}


int board_movePiece(Board* board, BoardSquare from, BoardSquare to)
{
    Piece piece;

    if (board == NULL)
    {
        return 0;
    }

    if (!board_isValidSquare(from) || !board_isValidSquare(to))
    {
        return 0;
    }

    piece = board_getPiece(board, from);

    if (piece_isEmpty(piece))
    {
        return 0;
    }

    board_setPiece(board, to, piece);
    board_setPiece(board, from, piece_empty());

    return 1;
}


void board_print(const Board* board, FILE* stream)
{
    board_printPerspective(board, stream, boardPerspectiveWhite);
}


void board_printPerspective(const Board* board, FILE* stream, BoardPerspective perspective)
{
    int row;
    int column;

    if (board == NULL || stream == NULL)
    {
        return;
    }

    for (row = 0; row < BOARD_SIZE; row++)
    {
        BoardViewSquare viewSquare;
        BoardSquare boardSquare;

        viewSquare = board_makeViewSquare(0, row);
        boardSquare = board_viewSquareToBoardSquare(viewSquare, perspective);

        fprintf(stream, "%d  ", boardSquare.rank + 1);

        for (column = 0; column < BOARD_SIZE; column++)
        {
            Piece piece;

            viewSquare = board_makeViewSquare(column, row);
            boardSquare = board_viewSquareToBoardSquare(viewSquare, perspective);
            piece = board_getPiece(board, boardSquare);

            fprintf(stream, "%c ", piece_toFenChar(piece));
        }

        fprintf(stream, "\n");
    }

    fprintf(stream, "\n   ");

    for (column = 0; column < BOARD_SIZE; column++)
    {
        BoardViewSquare viewSquare;
        BoardSquare boardSquare;

        viewSquare = board_makeViewSquare(column, BOARD_SIZE - 1);
        boardSquare = board_viewSquareToBoardSquare(viewSquare, perspective);

        fprintf(stream, "%c ", (char)('a' + boardSquare.file));
    }

    fprintf(stream, "\n");
}