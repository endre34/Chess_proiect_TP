#include "frontend/game/chess_board_view.h"

#include "frontend/ui/display_field.h"

#include <stdlib.h>

#define BOARD_TEXTURE_TILE_SIZE 256

struct ChessBoardView
{
    const sfTexture* boardTexture;
    const sfTexture* piecesTexture;
    const sfFont* labelFont;

    GameSession* session;

    sfRectangleShape* squareShape;
    sfRectangleShape* highlightShape;
    sfSprite* pieceSprite;

    DisplayField* fileLabels[BOARD_SIZE];
    DisplayField* rankLabels[BOARD_SIZE];

    sfFloatRect bounds;
    float squareSize;

    sfBool hasSelectedSquare;
    BoardSquare selectedSquare;
    MoveList selectedMoves;
};

static const sfIntRect BOARD_LIGHT_RECT = {
    0,
    0,
    BOARD_TEXTURE_TILE_SIZE,
    BOARD_TEXTURE_TILE_SIZE
};

static const sfIntRect BOARD_DARK_RECT = {
    BOARD_TEXTURE_TILE_SIZE,
    0,
    BOARD_TEXTURE_TILE_SIZE,
    BOARD_TEXTURE_TILE_SIZE
};

static const sfIntRect PIECE_RECTS[2][7] =
{
    {
        {0, 0, 0, 0},
        {884, 0, 127, 185},
        {561, 0, 174, 185},
        {380, 0, 181, 185},
        {736, 0, 148, 185},
        {181, 0, 199, 185},
        {0, 0, 181, 185}
    },
    {
        {0, 0, 0, 0},
        {884, 185, 127, 186},
        {561, 185, 174, 186},
        {380, 185, 181, 186},
        {736, 185, 148, 186},
        {181, 185, 199, 186},
        {0, 185, 181, 186}
    }
};

static float minFloat(float left, float right)
{
    if (left < right)
        return left;

    return right;
}

static int pieceColorIndex(PieceColor color)
{
    if (color == pieceColorBlack)
        return 1;

    return 0;
}

static sfIntRect pieceTextureRect(Piece piece)
{
    if (piece.type <= pieceTypeNone || piece.type > pieceTypeKing)
        return (sfIntRect){0, 0, 0, 0};

    if (piece.color != pieceColorWhite && piece.color != pieceColorBlack)
        return (sfIntRect){0, 0, 0, 0};

    return PIECE_RECTS[pieceColorIndex(piece.color)][piece.type];
}

static sfVector2f viewSquarePosition(const ChessBoardView* view, BoardViewSquare square)
{
    return (sfVector2f){
        view->bounds.left + (float)square.column * view->squareSize,
        view->bounds.top + (float)square.row * view->squareSize
    };
}

static sfFloatRect viewSquareBounds(const ChessBoardView* view, BoardViewSquare square)
{
    sfVector2f position;

    position = viewSquarePosition(view, square);

    return (sfFloatRect){
        position.x,
        position.y,
        view->squareSize,
        view->squareSize
    };
}

static void setupLabel(DisplayField* label, const sfFont* font)
{
    displayField_setTextFont(label, font);
    displayField_setTextColor(label, sfWhite);
    displayField_setFillColor(label, (sfColor){0, 0, 0, 0});
    displayField_setOutlineThickness(label, 0.0f);
    displayField_setTextPadding(label, (sfVector2f){0.0f, 0.0f});
    displayField_setTextAlignment(
        label,
        displayFieldTextAlignCenter,
        displayFieldTextAlignMiddle
    );
}

static sfBool createLabels(ChessBoardView* view)
{
    int i;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        view->fileLabels[i] = displayField_create();
        view->rankLabels[i] = displayField_create();

        if (view->fileLabels[i] == NULL || view->rankLabels[i] == NULL)
            return sfFalse;

        setupLabel(view->fileLabels[i], view->labelFont);
        setupLabel(view->rankLabels[i], view->labelFont);
    }

    return sfTrue;
}

static void destroyLabels(ChessBoardView* view)
{
    int i;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (view->fileLabels[i] != NULL)
            displayField_destroy(view->fileLabels[i]);

        if (view->rankLabels[i] != NULL)
            displayField_destroy(view->rankLabels[i]);
    }
}

static void updateLabels(const ChessBoardView* view)
{
    BoardPerspective perspective;
    BoardViewSquare viewSquare;
    BoardSquare boardSquare;
    char text[2];
    sfVector2f labelSize;
    float labelGap;
    int i;

    if (view->session == NULL)
    {
        perspective = boardPerspectiveWhite;
    }
    else
    {
        perspective = gameSession_getPerspective(view->session);
    }

    labelSize = (sfVector2f){view->squareSize * 0.34f, view->squareSize * 0.34f};
    labelGap = view->squareSize * 0.10f;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        viewSquare = board_makeViewSquare(i, BOARD_SIZE - 1);
        boardSquare = board_viewSquareToBoardSquare(viewSquare, perspective);

        text[0] = (char)('a' + boardSquare.file);
        text[1] = '\0';

        displayField_setTextString(view->fileLabels[i], text);
        displayField_setCharacterSize(view->fileLabels[i], (unsigned int)(view->squareSize * 0.22f));
        displayField_setSize(view->fileLabels[i], labelSize);
        displayField_setOrigin(view->fileLabels[i], (sfVector2f){labelSize.x / 2.0f, labelSize.y / 2.0f});

        displayField_setPosition(
            view->fileLabels[i],
            (sfVector2f){
                view->bounds.left + ((float)i + 0.5f) * view->squareSize,
                view->bounds.top + view->bounds.height + labelGap + labelSize.y / 2.0f
            }
        );

        viewSquare = board_makeViewSquare(0, i);
        boardSquare = board_viewSquareToBoardSquare(viewSquare, perspective);

        text[0] = (char)('1' + boardSquare.rank);
        text[1] = '\0';

        displayField_setTextString(view->rankLabels[i], text);
        displayField_setCharacterSize(view->rankLabels[i], (unsigned int)(view->squareSize * 0.22f));
        displayField_setSize(view->rankLabels[i], labelSize);
        displayField_setOrigin(view->rankLabels[i], (sfVector2f){labelSize.x / 2.0f, labelSize.y / 2.0f});

        displayField_setPosition(
            view->rankLabels[i],
            (sfVector2f){
                view->bounds.left - labelGap - labelSize.x / 2.0f,
                view->bounds.top + ((float)i + 0.5f) * view->squareSize
            }
        );
    }
}

static void drawSquare(sfRenderWindow* window, const ChessBoardView* view, BoardViewSquare viewSquare)
{
    BoardPerspective perspective;
    BoardSquare boardSquare;
    sfVector2f position;

    if (view->session == NULL)
        perspective = boardPerspectiveWhite;
    else
        perspective = gameSession_getPerspective(view->session);

    boardSquare = board_viewSquareToBoardSquare(viewSquare, perspective);
    position = viewSquarePosition(view, viewSquare);

    sfRectangleShape_setPosition(view->squareShape, position);
    sfRectangleShape_setSize(view->squareShape, (sfVector2f){view->squareSize, view->squareSize});
    sfRectangleShape_setTexture(view->squareShape, view->boardTexture, sfFalse);

    if (board_squareColor(boardSquare) == 0)
        sfRectangleShape_setTextureRect(view->squareShape, BOARD_DARK_RECT);
    else
        sfRectangleShape_setTextureRect(view->squareShape, BOARD_LIGHT_RECT);

    sfRenderWindow_drawRectangleShape(window, view->squareShape, NULL);
}

static void drawSquares(sfRenderWindow* window, const ChessBoardView* view)
{
    int row;
    int column;

    for (row = 0; row < BOARD_SIZE; row++)
    {
        for (column = 0; column < BOARD_SIZE; column++)
        {
            drawSquare(window, view, board_makeViewSquare(column, row));
        }
    }
}

static void drawHighlightRect(sfRenderWindow* window, const ChessBoardView* view, BoardSquare square, sfColor color)
{
    BoardPerspective perspective;
    BoardViewSquare viewSquare;
    sfFloatRect bounds;

    if (!board_isValidSquare(square))
        return;

    if (view->session == NULL)
        perspective = boardPerspectiveWhite;
    else
        perspective = gameSession_getPerspective(view->session);

    viewSquare = board_boardSquareToViewSquare(square, perspective);
    bounds = viewSquareBounds(view, viewSquare);

    sfRectangleShape_setPosition(view->highlightShape, (sfVector2f){bounds.left, bounds.top});
    sfRectangleShape_setSize(view->highlightShape, (sfVector2f){bounds.width, bounds.height});
    sfRectangleShape_setFillColor(view->highlightShape, color);

    sfRenderWindow_drawRectangleShape(window, view->highlightShape, NULL);
}

static void drawHighlights(sfRenderWindow* window, const ChessBoardView* view)
{
    int i;

    if (!view->hasSelectedSquare)
        return;

    drawHighlightRect(window, view, view->selectedSquare, (sfColor){80, 160, 255, 85});

    for (i = 0; i < view->selectedMoves.count; i++)
        drawHighlightRect(window, view, view->selectedMoves.moves[i].to, (sfColor){80, 255, 120, 70});
}

static sfBool canSelectPiece(const ChessBoardView* view, BoardSquare square)
{
    const Game* game;
    const Board* board;
    Piece piece;
    PieceColor sideToMove;

    if (view->session == NULL)
        return sfFalse;

    game = gameSession_getGame(view->session);

    if (game == NULL)
        return sfFalse;

    if (gameSession_getStatus(view->session) != gameSessionStatusActive)
        return sfFalse;

    sideToMove = game_getSideToMove(game);

    if (gameSession_getMode(view->session) == gameSessionModeEnginePve)
    {
        if (sideToMove != gameSession_getHumanColor(view->session))
            return sfFalse;
    }

    board = game_getBoard(game);
    piece = board_getPiece(board, square);

    return !piece_isEmpty(piece) && piece.color == sideToMove;
}

static void selectSquare(ChessBoardView* view, BoardSquare square)
{
    const Game* game;

    game = gameSession_getGame(view->session);

    view->selectedSquare = square;
    view->hasSelectedSquare = sfTrue;
    moveList_clear(&view->selectedMoves);

    if (game != NULL)
        game_getLegalMovesFrom(game, square, &view->selectedMoves);
}

static void handleBoardClick(ChessBoardView* view, BoardSquare square)
{
    if (view->session == NULL)
        return;

    if (!view->hasSelectedSquare)
    {
        if (canSelectPiece(view, square))
            selectSquare(view, square);

        return;
    }

    if (moveList_containsToSquare(&view->selectedMoves, square))
    {
        gameSession_makeMove(view->session, view->selectedSquare, square);
        chessBoardView_clearSelection(view);
        return;
    }

    if (canSelectPiece(view, square))
    {
        selectSquare(view, square);
        return;
    }

    chessBoardView_clearSelection(view);
}

ChessBoardView* chessBoardView_create(const Resources* resources)
{
    ChessBoardView* view;
    int i;

    if (resources == NULL)
        return NULL;

    view = malloc(sizeof(ChessBoardView));

    if (view == NULL)
        return NULL;

    view->boardTexture = resources_getTexture(resources, resourceTextureBoard);
    view->piecesTexture = resources_getTexture(resources, resourceTexturePieces);
    view->labelFont = resources_getFont(resources, resourceFontJetBrainsMonoSemiBold);

    view->session = NULL;
    view->squareShape = NULL;
    view->highlightShape = NULL;
    view->pieceSprite = NULL;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        view->fileLabels[i] = NULL;
        view->rankLabels[i] = NULL;
    }

    view->bounds = (sfFloatRect){0.0f, 0.0f, 0.0f, 0.0f};
    view->squareSize = 0.0f;
    view->hasSelectedSquare = sfFalse;
    view->selectedSquare = board_invalidSquare();
    moveList_clear(&view->selectedMoves);

    view->squareShape = sfRectangleShape_create();
    view->highlightShape = sfRectangleShape_create();
    view->pieceSprite = sfSprite_create();

    if (
        view->squareShape == NULL ||
        view->highlightShape == NULL ||
        view->pieceSprite == NULL ||
        createLabels(view) == sfFalse
    )
    {
        chessBoardView_destroy(view);
        return NULL;
    }

    sfSprite_setTexture(view->pieceSprite, view->piecesTexture, sfFalse);

    return view;
}

void chessBoardView_destroy(ChessBoardView* view)
{
    if (view == NULL)
        return;

    destroyLabels(view);

    if (view->squareShape != NULL)
        sfRectangleShape_destroy(view->squareShape);

    if (view->highlightShape != NULL)
        sfRectangleShape_destroy(view->highlightShape);

    if (view->pieceSprite != NULL)
        sfSprite_destroy(view->pieceSprite);

    free(view);
}

void chessBoardView_setSession(ChessBoardView* view, GameSession* session)
{
    if (view == NULL)
        return;

    view->session = session;
    chessBoardView_clearSelection(view);
}

GameSession* chessBoardView_getSession(const ChessBoardView* view)
{
    if (view == NULL)
        return NULL;

    return view->session;
}

void chessBoardView_setBounds(ChessBoardView* view, sfFloatRect bounds)
{
    if (view == NULL)
        return;

    view->bounds = bounds;
    view->squareSize = bounds.width / (float)BOARD_SIZE;
}

sfFloatRect chessBoardView_getBounds(const ChessBoardView* view)
{
    if (view == NULL)
        return (sfFloatRect){0.0f, 0.0f, 0.0f, 0.0f};

    return view->bounds;
}

void chessBoardView_clearSelection(ChessBoardView* view)
{
    if (view == NULL)
        return;

    view->hasSelectedSquare = sfFalse;
    view->selectedSquare = board_invalidSquare();
    moveList_clear(&view->selectedMoves);
}

sfBool chessBoardView_getSquareAtPoint(const ChessBoardView* view, sfVector2i point, BoardSquare* square)
{
    int column;
    int row;
    BoardPerspective perspective;
    BoardViewSquare viewSquare;

    if (view == NULL || square == NULL)
        return sfFalse;

    if (
        (float)point.x < view->bounds.left ||
        (float)point.y < view->bounds.top ||
        (float)point.x >= view->bounds.left + view->bounds.width ||
        (float)point.y >= view->bounds.top + view->bounds.height
    )
    {
        return sfFalse;
    }

    column = (int)(((float)point.x - view->bounds.left) / view->squareSize);
    row = (int)(((float)point.y - view->bounds.top) / view->squareSize);

    viewSquare = board_makeViewSquare(column, row);

    if (view->session == NULL)
        perspective = boardPerspectiveWhite;
    else
        perspective = gameSession_getPerspective(view->session);

    *square = board_viewSquareToBoardSquare(viewSquare, perspective);

    return board_isValidSquare(*square);
}

void chessBoardView_updateMouse(ChessBoardView* view, const Mouse* mouse)
{
    BoardSquare square;

    if (view == NULL || mouse == NULL)
        return;

    if (!mouse_wasJustPressed(mouse))
        return;

    if (!chessBoardView_getSquareAtPoint(view, mouse_getPosition(mouse), &square))
        return;

    handleBoardClick(view, square);
}

void chessBoardView_drawPieceIcon(sfRenderWindow* window, const ChessBoardView* view, Piece piece, sfFloatRect area)
{
    sfIntRect textureRect;
    float scale;

    if (window == NULL || view == NULL)
        return;

    if (piece_isEmpty(piece))
        return;

    textureRect = pieceTextureRect(piece);

    if (textureRect.width <= 0 || textureRect.height <= 0)
        return;

    scale = minFloat(
        area.width / (float)textureRect.width,
        area.height / (float)textureRect.height
    ) * 0.88f;

    sfSprite_setTexture(view->pieceSprite, view->piecesTexture, sfFalse);
    sfSprite_setTextureRect(view->pieceSprite, textureRect);
    sfSprite_setOrigin(
        view->pieceSprite,
        (sfVector2f){(float)textureRect.width / 2.0f, (float)textureRect.height / 2.0f}
    );
    sfSprite_setScale(view->pieceSprite, (sfVector2f){scale, scale});
    sfSprite_setPosition(
        view->pieceSprite,
        (sfVector2f){area.left + area.width / 2.0f, area.top + area.height / 2.0f}
    );

    sfRenderWindow_drawSprite(window, view->pieceSprite, NULL);
}

void chessBoardView_draw(sfRenderWindow* window, const ChessBoardView* view)
{
    const Game* game;
    const Board* board;
    BoardPerspective perspective;
    BoardViewSquare viewSquare;
    BoardSquare boardSquare;
    Piece piece;
    int row;
    int column;

    if (window == NULL || view == NULL)
        return;

    drawSquares(window, view);
    drawHighlights(window, view);

    game = NULL;
    board = NULL;

    if (view->session != NULL)
        game = gameSession_getGame(view->session);

    if (game != NULL)
        board = game_getBoard(game);

    if (view->session == NULL)
        perspective = boardPerspectiveWhite;
    else
        perspective = gameSession_getPerspective(view->session);

    if (board != NULL)
    {
        for (row = 0; row < BOARD_SIZE; row++)
        {
            for (column = 0; column < BOARD_SIZE; column++)
            {
                viewSquare = board_makeViewSquare(column, row);
                boardSquare = board_viewSquareToBoardSquare(viewSquare, perspective);
                piece = board_getPiece(board, boardSquare);

                chessBoardView_drawPieceIcon(window, view, piece, viewSquareBounds(view, viewSquare));
            }
        }
    }

    updateLabels(view);

    for (row = 0; row < BOARD_SIZE; row++)
        displayField_draw(window, view->rankLabels[row]);

    for (column = 0; column < BOARD_SIZE; column++)
        displayField_draw(window, view->fileLabels[column]);
}