#include "frontend/screens/game_local_pvp_screen.h"

#include "frontend/game/chess_board_view.h"
#include "frontend/ui/display_field.h"

#include <stdio.h>
#include <stdlib.h>

#define SIDEBOARD_TEXTURE_WIDTH 1596
#define SIDEBOARD_TEXTURE_HEIGHT 896

#define TIME_BOARD_TEXTURE_WIDTH 1196
#define TIME_BOARD_TEXTURE_HEIGHT 316

#define STARTING_PAWN_COUNT 8
#define STARTING_MINOR_COUNT 2
#define STARTING_ROOK_COUNT 2
#define STARTING_QUEEN_COUNT 1


struct gameLocalPvpScreen
{
    ChessBoardView* boardView;

    DisplayField* topSideboard;
    DisplayField* bottomSideboard;

    DisplayField* topTimer;
    DisplayField* bottomTimer;
    DisplayField* statusField;

    GameSession* session;
    LocalPvPSetup setup;

    sfBool active;
    gameLocalPvpScreenAction action;
};


static const sfIntRect SIDEBOARD_TEXTURE_RECT = {
    0,
    0,
    SIDEBOARD_TEXTURE_WIDTH,
    SIDEBOARD_TEXTURE_HEIGHT
};


static const sfIntRect TIME_BOARD_TEXTURE_RECT = {
    0,
    0,
    TIME_BOARD_TEXTURE_WIDTH,
    TIME_BOARD_TEXTURE_HEIGHT
};


static void gameLocalPvpScreen_applyResources(gameLocalPvpScreen*, const Resources*);
static void gameLocalPvpScreen_setLayout(gameLocalPvpScreen*, sfVector2i, sfVector2i);
static void gameLocalPvpScreen_updateTexts(gameLocalPvpScreen*);


static float minFloat(float left, float right)
{
    if (left < right)
    {
        return left;
    }

    return right;
}


static void setDisplayFieldRect(DisplayField* field, sfFloatRect rect)
{
    displayField_setSize(field, (sfVector2f){rect.width, rect.height});
    displayField_setOrigin(field, (sfVector2f){rect.width / 2.0f, rect.height / 2.0f});
    displayField_setPosition(field, (sfVector2f){rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f});
}


static void setupSideboard(DisplayField* field, const Resources* resources)
{
    displayField_setTexture(field, resources_getTexture(resources, resourceTextureSideboard), sfFalse);
    displayField_setTextureRect(field, SIDEBOARD_TEXTURE_RECT);
    displayField_setFillColor(field, sfWhite);
    displayField_setOutlineThickness(field, 0.0f);
}


static void setupTimer(DisplayField* field, const Resources* resources)
{
    displayField_setTexture(field, resources_getTexture(resources, resourceTextureTimeBoard), sfFalse);
    displayField_setTextureRect(field, TIME_BOARD_TEXTURE_RECT);
    displayField_setTextFont(field, resources_getFont(resources, resourceFontJetBrainsMonoSemiBold));
    displayField_setTextColor(field, sfWhite);
    displayField_setCharacterSize(field, 34);
    displayField_setLetterSpacing(field, 1.0f);
    displayField_setTextPadding(field, (sfVector2f){0.0f, 0.0f});
    displayField_setTextAlignment(field, displayFieldTextAlignCenter, displayFieldTextAlignMiddle);
    displayField_setFillColor(field, sfWhite);
    displayField_setOutlineThickness(field, 0.0f);
}


static void setupStatusField(DisplayField* field, const Resources* resources)
{
    displayField_setTextFont(field, resources_getFont(resources, resourceFontCinzelSemiBold));
    displayField_setTextColor(field, sfWhite);
    displayField_setCharacterSize(field, 28);
    displayField_setLetterSpacing(field, 1.0f);
    displayField_setTextPadding(field, (sfVector2f){0.0f, 0.0f});
    displayField_setTextAlignment(field, displayFieldTextAlignCenter, displayFieldTextAlignMiddle);
    displayField_setFillColor(field, (sfColor){0, 0, 0, 0});
    displayField_setOutlineThickness(field, 0.0f);
}


static void writeDefaultTimeText(char* buffer, int bufferSize, unsigned int seconds)
{
    unsigned int minutes;
    unsigned int remainder;

    minutes = seconds / 60;
    remainder = seconds % 60;

    snprintf(buffer, bufferSize, "%u:%02u", minutes, remainder);
}


static const char* pieceColorName(PieceColor color)
{
    if (color == pieceColorWhite)
    {
        return "White";
    }

    if (color == pieceColorBlack)
    {
        return "Black";
    }

    return "None";
}


static PieceColor bottomColor(const gameLocalPvpScreen* screen)
{
    BoardPerspective perspective;

    if (screen == NULL || screen->session == NULL)
    {
        return pieceColorWhite;
    }

    perspective = gameSession_getPerspective(screen->session);

    if (perspective == boardPerspectiveBlack)
    {
        return pieceColorBlack;
    }

    return pieceColorWhite;
}


static PieceColor topColor(const gameLocalPvpScreen* screen)
{
    return piece_oppositeColor(bottomColor(screen));
}


static void writeStatusText(const gameLocalPvpScreen* screen, char* buffer, int bufferSize)
{
    const Game* game;
    PieceColor sideToMove;
    GameSessionStatus status;
    GameSessionEndReason reason;

    if (buffer == NULL || bufferSize <= 0)
    {
        return;
    }

    buffer[0] = '\0';

    if (screen == NULL || screen->session == NULL)
    {
        snprintf(buffer, bufferSize, "LOCAL PVP");
        return;
    }

    status = gameSession_getStatus(screen->session);
    reason = gameSession_getEndReason(screen->session);

    if (status == gameSessionStatusWhiteWon)
    {
        snprintf(buffer, bufferSize, "White won - %s", gameSessionEndReason_toString(reason));
        return;
    }

    if (status == gameSessionStatusBlackWon)
    {
        snprintf(buffer, bufferSize, "Black won - %s", gameSessionEndReason_toString(reason));
        return;
    }

    if (status == gameSessionStatusDraw)
    {
        snprintf(buffer, bufferSize, "Draw - %s", gameSessionEndReason_toString(reason));
        return;
    }

    game = gameSession_getGame(screen->session);

    if (game == NULL)
    {
        snprintf(buffer, bufferSize, "LOCAL PVP");
        return;
    }

    sideToMove = game_getSideToMove(game);

    if (game_isInCheck(game, sideToMove))
    {
        snprintf(buffer, bufferSize, "%s in check", pieceColorName(sideToMove));
        return;
    }

    snprintf(buffer, bufferSize, "%s to move", pieceColorName(sideToMove));
}


static void gameLocalPvpScreen_applyResources(gameLocalPvpScreen* screen, const Resources* resources)
{
    setupSideboard(screen->topSideboard, resources);
    setupSideboard(screen->bottomSideboard, resources);

    setupTimer(screen->topTimer, resources);
    setupTimer(screen->bottomTimer, resources);
    setupStatusField(screen->statusField, resources);
}


static void gameLocalPvpScreen_updateTexts(gameLocalPvpScreen* screen)
{
    char topTime[GAME_CLOCK_TEXT_MAX];
    char bottomTime[GAME_CLOCK_TEXT_MAX];
    char statusText[128];
    const GameClock* clock;

    if (screen == NULL)
    {
        return;
    }

    if (screen->session == NULL)
    {
        writeDefaultTimeText(topTime, GAME_CLOCK_TEXT_MAX, screen->setup.startingTimeSeconds);
        writeDefaultTimeText(bottomTime, GAME_CLOCK_TEXT_MAX, screen->setup.startingTimeSeconds);
        snprintf(statusText, 128, "LOCAL PVP");
    }
    else
    {
        clock = gameSession_getClock(screen->session);

        if (gameClock_hasTimeControl(clock))
        {
            gameSession_writeTime(screen->session, topColor(screen), topTime, GAME_CLOCK_TEXT_MAX);
            gameSession_writeTime(screen->session, bottomColor(screen), bottomTime, GAME_CLOCK_TEXT_MAX);
        }
        else
        {
            snprintf(topTime, GAME_CLOCK_TEXT_MAX, "--:--");
            snprintf(bottomTime, GAME_CLOCK_TEXT_MAX, "--:--");
        }

        writeStatusText(screen, statusText, 128);
    }

    displayField_setTextString(screen->topTimer, topTime);
    displayField_setTextString(screen->bottomTimer, bottomTime);
    displayField_setTextString(screen->statusField, statusText);
}


static void gameLocalPvpScreen_setLayout(gameLocalPvpScreen* screen, sfVector2i topLeft, sfVector2i bottomRight)
{
    float areaWidth;
    float areaHeight;
    float boardColumnWidth;
    float sideColumnWidth;
    float boardSize;
    float boardX;
    float boardY;
    float sideX;
    float sideWidth;
    float sideMargin;
    float sideboardHeight;
    float timerHeight;
    sfFloatRect topSideboardRect;
    sfFloatRect bottomSideboardRect;
    sfFloatRect topTimerRect;
    sfFloatRect bottomTimerRect;
    sfFloatRect statusRect;

    areaWidth = (float)(bottomRight.x - topLeft.x);
    areaHeight = (float)(bottomRight.y - topLeft.y);

    sideColumnWidth = areaWidth * 0.26f;
    boardColumnWidth = areaWidth - sideColumnWidth;

    boardSize = minFloat(boardColumnWidth * 0.92f, areaHeight * 0.92f);
    boardX = (float)topLeft.x + (boardColumnWidth - boardSize) / 2.0f;
    boardY = (float)topLeft.y + (areaHeight - boardSize) / 2.0f;

    chessBoardView_setBounds(screen->boardView, (sfFloatRect){boardX, boardY, boardSize, boardSize});

    sideMargin = sideColumnWidth * 0.08f;
    sideX = (float)topLeft.x + boardColumnWidth + sideMargin;
    sideWidth = sideColumnWidth - 2.0f * sideMargin;

    sideboardHeight = areaHeight * 0.30f;
    timerHeight = areaHeight * 0.09f;

    topSideboardRect = (sfFloatRect){
        sideX,
        (float)topLeft.y + areaHeight * 0.05f,
        sideWidth,
        sideboardHeight
    };

    topTimerRect = (sfFloatRect){
        sideX,
        (float)topLeft.y + areaHeight * 0.38f,
        sideWidth,
        timerHeight
    };

    statusRect = (sfFloatRect){
        sideX,
        (float)topLeft.y + areaHeight * 0.475f,
        sideWidth,
        areaHeight * 0.05f
    };

    bottomTimerRect = (sfFloatRect){
        sideX,
        (float)topLeft.y + areaHeight * 0.53f,
        sideWidth,
        timerHeight
    };

    bottomSideboardRect = (sfFloatRect){
        sideX,
        (float)topLeft.y + areaHeight * 0.65f,
        sideWidth,
        sideboardHeight
    };

    setDisplayFieldRect(screen->topSideboard, topSideboardRect);
    setDisplayFieldRect(screen->topTimer, topTimerRect);
    setDisplayFieldRect(screen->statusField, statusRect);
    setDisplayFieldRect(screen->bottomTimer, bottomTimerRect);
    setDisplayFieldRect(screen->bottomSideboard, bottomSideboardRect);
}


static int startingPieceCount(PieceType type)
{
    switch (type)
    {
        case pieceTypePawn:
            return STARTING_PAWN_COUNT;

        case pieceTypeKnight:
        case pieceTypeBishop:
        case pieceTypeRook:
            return STARTING_MINOR_COUNT;

        case pieceTypeQueen:
            return STARTING_QUEEN_COUNT;

        default:
            return 0;
    }
}


static void countBoardPieces(const Board* board, PieceColor color, int counts[7])
{
    BoardSquare square;
    Piece piece;
    int file;
    int rank;

    for (file = 0; file < 7; file++)
    {
        counts[file] = 0;
    }

    if (board == NULL)
    {
        return;
    }

    for (rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (file = 0; file < BOARD_SIZE; file++)
        {
            square = board_makeSquare(file, rank);
            piece = board_getPiece(board, square);

            if (!piece_isEmpty(piece) && piece.color == color)
            {
                counts[piece.type]++;
            }
        }
    }
}


static void drawCapturedPieces(sfRenderWindow* window, const gameLocalPvpScreen* screen, PieceColor capturedColor, sfFloatRect bounds)
{
    const Game* game;
    const Board* board;
    int counts[7];
    int type;
    int missing;
    int i;
    int drawn;
    int columns;
    float cellSize;
    float startX;
    float startY;
    sfFloatRect cell;

    if (screen->session == NULL)
    {
        return;
    }

    game = gameSession_getGame(screen->session);

    if (game == NULL)
    {
        return;
    }

    board = game_getBoard(game);
    countBoardPieces(board, capturedColor, counts);

    columns = 6;
    cellSize = bounds.width / (float)columns;
    startX = bounds.left;
    startY = bounds.top + bounds.height * 0.16f;
    drawn = 0;

    for (type = pieceTypeQueen; type >= pieceTypePawn; type--)
    {
        missing = startingPieceCount((PieceType)type) - counts[type];

        for (i = 0; i < missing; i++)
        {
            cell = (sfFloatRect){
                startX + (float)(drawn % columns) * cellSize,
                startY + ((float)drawn / columns) * cellSize,
                cellSize,
                cellSize
            };

            chessBoardView_drawPieceIcon(window, screen->boardView, piece_make((PieceType)type, capturedColor), cell);
            drawn++;
        }
    }
}


gameLocalPvpScreen* gameLocalPvpScreen_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    gameLocalPvpScreen* screen;

    if (resources == NULL)
    {
        return NULL;
    }

    screen = malloc(sizeof(gameLocalPvpScreen));

    if (screen == NULL)
    {
        return NULL;
    }

    screen->boardView = NULL;
    screen->topSideboard = NULL;
    screen->bottomSideboard = NULL;
    screen->topTimer = NULL;
    screen->bottomTimer = NULL;
    screen->statusField = NULL;

    screen->session = NULL;
    screen->setup = localPvPSetup_getDefault();
    screen->active = sfTrue;
    screen->action = gameLocalPvpScreenActionNone;

    screen->boardView = chessBoardView_create(resources);
    screen->topSideboard = displayField_create();
    screen->bottomSideboard = displayField_create();
    screen->topTimer = displayField_create();
    screen->bottomTimer = displayField_create();
    screen->statusField = displayField_create();

    if (
        screen->boardView == NULL ||
        screen->topSideboard == NULL ||
        screen->bottomSideboard == NULL ||
        screen->topTimer == NULL ||
        screen->bottomTimer == NULL ||
        screen->statusField == NULL
    )
    {
        gameLocalPvpScreen_destroy(screen);
        return NULL;
    }

    gameLocalPvpScreen_applyResources(screen, resources);
    gameLocalPvpScreen_updateTexts(screen);
    gameLocalPvpScreen_setLayout(screen, topLeft, bottomRight);

    return screen;
}


void gameLocalPvpScreen_destroy(gameLocalPvpScreen* screen)
{
    if (screen == NULL)
    {
        return;
    }

    if (screen->boardView != NULL)
    {
        chessBoardView_destroy(screen->boardView);
    }

    if (screen->topSideboard != NULL)
    {
        displayField_destroy(screen->topSideboard);
    }

    if (screen->bottomSideboard != NULL)
    {
        displayField_destroy(screen->bottomSideboard);
    }

    if (screen->topTimer != NULL)
    {
        displayField_destroy(screen->topTimer);
    }

    if (screen->bottomTimer != NULL)
    {
        displayField_destroy(screen->bottomTimer);
    }

    if (screen->statusField != NULL)
    {
        displayField_destroy(screen->statusField);
    }

    free(screen);
}


void gameLocalPvpScreen_update(gameLocalPvpScreen* screen, int elapsedMilliseconds)
{
    if (screen == NULL)
    {
        return;
    }

    if (!screen->active)
    {
        return;
    }

    if (screen->session != NULL)
    {
        gameSession_update(screen->session, elapsedMilliseconds);
    }

    gameLocalPvpScreen_updateTexts(screen);
}


void gameLocalPvpScreen_updateMouse(gameLocalPvpScreen* screen, const Mouse* mouse)
{
    if (screen == NULL || mouse == NULL)
    {
        return;
    }

    if (!screen->active)
    {
        return;
    }

    chessBoardView_updateMouse(screen->boardView, mouse);
    gameLocalPvpScreen_updateTexts(screen);
}


void gameLocalPvpScreen_setSession(gameLocalPvpScreen* screen, GameSession* session)
{
    if (screen == NULL)
    {
        return;
    }

    screen->session = session;
    chessBoardView_setSession(screen->boardView, session);
    gameLocalPvpScreen_updateTexts(screen);
}


void gameLocalPvpScreen_setSetup(gameLocalPvpScreen* screen, LocalPvPSetup setup)
{
    if (screen == NULL)
    {
        return;
    }

    screen->setup = setup;
    gameLocalPvpScreen_updateTexts(screen);
}


void gameLocalPvpScreen_setActive(gameLocalPvpScreen* screen, sfBool active)
{
    if (screen == NULL)
    {
        return;
    }

    screen->active = active;
}


sfBool gameLocalPvpScreen_isActive(const gameLocalPvpScreen* screen)
{
    if (screen == NULL)
    {
        return sfFalse;
    }

    return screen->active;
}


gameLocalPvpScreenAction gameLocalPvpScreen_getAction(const gameLocalPvpScreen* screen)
{
    if (screen == NULL)
    {
        return gameLocalPvpScreenActionNone;
    }

    return screen->action;
}


gameLocalPvpScreenAction gameLocalPvpScreen_consumeAction(gameLocalPvpScreen* screen)
{
    gameLocalPvpScreenAction action;

    if (screen == NULL)
    {
        return gameLocalPvpScreenActionNone;
    }

    action = screen->action;
    screen->action = gameLocalPvpScreenActionNone;

    return action;
}


void gameLocalPvpScreen_draw(sfRenderWindow* window, const gameLocalPvpScreen* screen)
{
    sfFloatRect topBounds;
    sfFloatRect bottomBounds;

    if (window == NULL || screen == NULL)
    {
        return;
    }

    if (!screen->active)
    {
        return;
    }

    chessBoardView_draw(window, screen->boardView);

    displayField_draw(window, screen->topSideboard);
    displayField_draw(window, screen->bottomSideboard);

    topBounds = displayField_getGlobalBounds(screen->topSideboard);
    bottomBounds = displayField_getGlobalBounds(screen->bottomSideboard);

    drawCapturedPieces(window, screen, topColor(screen), topBounds);
    drawCapturedPieces(window, screen, bottomColor(screen), bottomBounds);

    displayField_draw(window, screen->topTimer);
    displayField_draw(window, screen->statusField);
    displayField_draw(window, screen->bottomTimer);
}