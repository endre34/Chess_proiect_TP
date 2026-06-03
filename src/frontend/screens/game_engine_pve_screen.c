#include "frontend/screens/game_engine_pve_screen.h"

#include "frontend/game/chess_board_view.h"
#include "frontend/ui/display_field.h"

#include <stdio.h>
#include <stdlib.h>

#define SIDEBOARD_TEXTURE_WIDTH 1596
#define SIDEBOARD_TEXTURE_HEIGHT 896


struct gameEnginePveScreen
{
    ChessBoardView* boardView;

    DisplayField* sideboard;
    DisplayField* statusField;

    GameSession* session;

    sfBool active;
    gameEnginePveScreenAction action;
};


static const sfIntRect SIDEBOARD_TEXTURE_RECT = {
    0,
    0,
    SIDEBOARD_TEXTURE_WIDTH,
    SIDEBOARD_TEXTURE_HEIGHT
};


static void gameEnginePveScreen_applyResources(gameEnginePveScreen*, const Resources*);
static void gameEnginePveScreen_setLayout(gameEnginePveScreen*, sfVector2i, sfVector2i);
static void gameEnginePveScreen_updateStatus(gameEnginePveScreen*);


static float minFloat(float left, float right)
{
    if (left < right)
    {
        return left;
    }

    return right;
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


static void setupStatusField(DisplayField* field, const Resources* resources)
{
    displayField_setTextFont(field, resources_getFont(resources, resourceFontCinzelSemiBold));
    displayField_setTextColor(field, sfWhite);
    displayField_setCharacterSize(field, 28);
    displayField_setLetterSpacing(field, 1.1f);
    displayField_setTextPadding(field, (sfVector2f){0.0f, 0.0f});
    displayField_setTextAlignment(field, displayFieldTextAlignCenter, displayFieldTextAlignMiddle);
    displayField_setFillColor(field, (sfColor){0, 0, 0, 0});
    displayField_setOutlineThickness(field, 0.0f);
}


static void gameEnginePveScreen_applyResources(gameEnginePveScreen* screen, const Resources* resources)
{
    setupSideboard(screen->sideboard, resources);
    setupStatusField(screen->statusField, resources);
}


static void gameEnginePveScreen_setLayout(gameEnginePveScreen* screen, sfVector2i topLeft, sfVector2i bottomRight)
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
    sfFloatRect statusRect;
    sfFloatRect sideboardRect;

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

    statusRect = (sfFloatRect){
        sideX,
        (float)topLeft.y + areaHeight * 0.12f,
        sideWidth,
        areaHeight * 0.12f
    };

    sideboardRect = (sfFloatRect){
        sideX,
        (float)topLeft.y + areaHeight * 0.32f,
        sideWidth,
        areaHeight * 0.46f
    };

    setDisplayFieldRect(screen->statusField, statusRect);
    setDisplayFieldRect(screen->sideboard, sideboardRect);
}


static void gameEnginePveScreen_updateStatus(gameEnginePveScreen* screen)
{
    const Game* game;
    PieceColor sideToMove;
    PieceColor humanColor;
    GameSessionStatus status;
    GameSessionEndReason reason;
    char buffer[128];

    if (screen == NULL)
    {
        return;
    }

    if (screen->session == NULL)
    {
        displayField_setTextString(screen->statusField, "VS ENGINE");
        return;
    }

    status = gameSession_getStatus(screen->session);
    reason = gameSession_getEndReason(screen->session);

    if (status == gameSessionStatusWhiteWon)
    {
        snprintf(buffer, 128, "White won - %s", gameSessionEndReason_toString(reason));
        displayField_setTextString(screen->statusField, buffer);
        return;
    }

    if (status == gameSessionStatusBlackWon)
    {
        snprintf(buffer, 128, "Black won - %s", gameSessionEndReason_toString(reason));
        displayField_setTextString(screen->statusField, buffer);
        return;
    }

    if (status == gameSessionStatusDraw)
    {
        snprintf(buffer, 128, "Draw - %s", gameSessionEndReason_toString(reason));
        displayField_setTextString(screen->statusField, buffer);
        return;
    }

    game = gameSession_getGame(screen->session);

    if (game == NULL)
    {
        displayField_setTextString(screen->statusField, "VS ENGINE");
        return;
    }

    sideToMove = game_getSideToMove(game);
    humanColor = gameSession_getHumanColor(screen->session);

    if (game_isInCheck(game, sideToMove))
    {
        snprintf(buffer, 128, "%s in check", pieceColorName(sideToMove));
        displayField_setTextString(screen->statusField, buffer);
        return;
    }

    if (sideToMove == humanColor)
    {
        displayField_setTextString(screen->statusField, "Your move");
        return;
    }

    displayField_setTextString(screen->statusField, "Engine to move");
}


gameEnginePveScreen* gameEnginePveScreen_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    gameEnginePveScreen* screen;

    if (resources == NULL)
    {
        return NULL;
    }

    screen = malloc(sizeof(gameEnginePveScreen));

    if (screen == NULL)
    {
        return NULL;
    }

    screen->boardView = NULL;
    screen->sideboard = NULL;
    screen->statusField = NULL;

    screen->session = NULL;
    screen->active = sfTrue;
    screen->action = gameEnginePveScreenActionNone;

    screen->boardView = chessBoardView_create(resources);
    screen->sideboard = displayField_create();
    screen->statusField = displayField_create();

    if (
        screen->boardView == NULL ||
        screen->sideboard == NULL ||
        screen->statusField == NULL
    )
    {
        gameEnginePveScreen_destroy(screen);
        return NULL;
    }

    gameEnginePveScreen_applyResources(screen, resources);
    gameEnginePveScreen_updateStatus(screen);
    gameEnginePveScreen_setLayout(screen, topLeft, bottomRight);

    return screen;
}


void gameEnginePveScreen_destroy(gameEnginePveScreen* screen)
{
    if (screen == NULL)
    {
        return;
    }

    if (screen->boardView != NULL)
    {
        chessBoardView_destroy(screen->boardView);
    }

    if (screen->sideboard != NULL)
    {
        displayField_destroy(screen->sideboard);
    }

    if (screen->statusField != NULL)
    {
        displayField_destroy(screen->statusField);
    }

    free(screen);
}


void gameEnginePveScreen_update(gameEnginePveScreen* screen, int elapsedMilliseconds)
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

    gameEnginePveScreen_updateStatus(screen);
}


void gameEnginePveScreen_updateMouse(gameEnginePveScreen* screen, const Mouse* mouse)
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
    gameEnginePveScreen_updateStatus(screen);
}


void gameEnginePveScreen_setSession(gameEnginePveScreen* screen, GameSession* session)
{
    if (screen == NULL)
    {
        return;
    }

    screen->session = session;
    chessBoardView_setSession(screen->boardView, session);
    gameEnginePveScreen_updateStatus(screen);
}


void gameEnginePveScreen_setActive(gameEnginePveScreen* screen, sfBool active)
{
    if (screen == NULL)
    {
        return;
    }

    screen->active = active;
}


sfBool gameEnginePveScreen_isActive(const gameEnginePveScreen* screen)
{
    if (screen == NULL)
    {
        return sfFalse;
    }

    return screen->active;
}


gameEnginePveScreenAction gameEnginePveScreen_getAction(const gameEnginePveScreen* screen)
{
    if (screen == NULL)
    {
        return gameEnginePveScreenActionNone;
    }

    return screen->action;
}


gameEnginePveScreenAction gameEnginePveScreen_consumeAction(gameEnginePveScreen* screen)
{
    gameEnginePveScreenAction action;

    if (screen == NULL)
    {
        return gameEnginePveScreenActionNone;
    }

    action = screen->action;
    screen->action = gameEnginePveScreenActionNone;

    return action;
}


void gameEnginePveScreen_draw(sfRenderWindow* window, const gameEnginePveScreen* screen)
{
    if (window == NULL || screen == NULL)
    {
        return;
    }

    if (!screen->active)
    {
        return;
    }

    chessBoardView_draw(window, screen->boardView);

    displayField_draw(window, screen->statusField);
    displayField_draw(window, screen->sideboard);
}