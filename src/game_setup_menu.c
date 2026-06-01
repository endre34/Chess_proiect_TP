#include "game_setup_menu.h"

#include "button.h"
#include "display_field.h"

#include <stdlib.h>

#define MENU_BUTTON_TEXTURE_WIDTH 916
#define MENU_BUTTON_TEXTURE_HEIGHT 240

#define MENU_TITLE_TEXTURE_WIDTH 1600
#define MENU_TITLE_TEXTURE_HEIGHT 320

struct gameSetupMenu
{
    DisplayField* titlebar;
    DisplayField* info;

    Button* localPvP;
    Button* vsEngine;
    Button* networkPvP;
    Button* back;

    sfBool active;

    GameSetupMenuAction action;
};

static const sfIntRect MENU_BUTTON_IDLE_RECT = {
    0,
    0,
    MENU_BUTTON_TEXTURE_WIDTH,
    MENU_BUTTON_TEXTURE_HEIGHT
};

static const sfIntRect MENU_BUTTON_HOVER_RECT = {
    MENU_BUTTON_TEXTURE_WIDTH,
    0,
    MENU_BUTTON_TEXTURE_WIDTH,
    MENU_BUTTON_TEXTURE_HEIGHT
};

static const sfIntRect MENU_BUTTON_PRESS_RECT = {
    2 * MENU_BUTTON_TEXTURE_WIDTH,
    0,
    MENU_BUTTON_TEXTURE_WIDTH,
    MENU_BUTTON_TEXTURE_HEIGHT
};

static const sfIntRect MENU_TITLE_RECT = {
    0,
    0,
    MENU_TITLE_TEXTURE_WIDTH,
    MENU_TITLE_TEXTURE_HEIGHT
};

static void gameSetupMenu_onLocalPvP(void*);
static void gameSetupMenu_onVsEngine(void*);
static void gameSetupMenu_onNetworkPvP(void*);
static void gameSetupMenu_onBack(void*);

static void gameSetupMenu_applyResources(gameSetupMenu*, const Resources*);
static void gameSetupMenu_setTexts(gameSetupMenu*);
static void gameSetupMenu_setStyle(gameSetupMenu*);
static void gameSetupMenu_setLayout(gameSetupMenu*, sfVector2i, sfVector2i);
static void gameSetupMenu_setActions(gameSetupMenu*);

static void gameSetupMenu_setupButtonTexture(Button*, const sfTexture*);
static void gameSetupMenu_setupButtonText(Button*, const sfFont*);

static void gameSetupMenu_onLocalPvP(void* data)
{
    ((gameSetupMenu*)data)->action = gameSetupMenuActionLocalPvP;
}

static void gameSetupMenu_onVsEngine(void* data)
{
    ((gameSetupMenu*)data)->action = gameSetupMenuActionVsEngine;
}

static void gameSetupMenu_onNetworkPvP(void* data)
{
    ((gameSetupMenu*)data)->action = gameSetupMenuActionNetworkPvP;
}

static void gameSetupMenu_onBack(void* data)
{
    ((gameSetupMenu*)data)->action = gameSetupMenuActionBack;
}

static void gameSetupMenu_setupButtonTexture(Button* button, const sfTexture* texture)
{
    button_setTexture(button, texture, sfFalse);

    button_setTextureRect_onIdle(button, MENU_BUTTON_IDLE_RECT);
    button_setTextureRect_onHover(button, MENU_BUTTON_HOVER_RECT);
    button_setTextureRect_onPress(button, MENU_BUTTON_PRESS_RECT);
}

static void gameSetupMenu_setupButtonText(Button* button, const sfFont* font)
{
    button_setTextFont(button, font);
    button_setTextColor(button, sfWhite);
    button_setCharacterSize(button, 30);
    button_setLetterSpacing(button, 1.25f);
}

static void gameSetupMenu_applyResources(gameSetupMenu* menu, const Resources* resources)
{
    const sfTexture* buttonTexture;
    const sfTexture* titleBoardTexture;

    const sfFont* titleFont;
    const sfFont* buttonFont;
    const sfFont* infoFont;

    buttonTexture = resources_getTexture(resources, resourceTextureButtons);
    titleBoardTexture = resources_getTexture(resources, resourceTextureTitleBoard);

    titleFont = resources_getFont(resources, resourceFontCinzelSemiBold);
    buttonFont = resources_getFont(resources, resourceFontCinzelMedium);
    infoFont = resources_getFont(resources, resourceFontJetBrainsMonoRegular);

    displayField_setTexture(menu->titlebar, titleBoardTexture, sfFalse);
    displayField_setTextureRect(menu->titlebar, MENU_TITLE_RECT);
    displayField_setTextFont(menu->titlebar, titleFont);

    displayField_setTextFont(menu->info, infoFont);

    gameSetupMenu_setupButtonTexture(menu->localPvP, buttonTexture);
    gameSetupMenu_setupButtonTexture(menu->vsEngine, buttonTexture);
    gameSetupMenu_setupButtonTexture(menu->networkPvP, buttonTexture);
    gameSetupMenu_setupButtonTexture(menu->back, buttonTexture);

    gameSetupMenu_setupButtonText(menu->localPvP, buttonFont);
    gameSetupMenu_setupButtonText(menu->vsEngine, buttonFont);
    gameSetupMenu_setupButtonText(menu->networkPvP, buttonFont);
    gameSetupMenu_setupButtonText(menu->back, buttonFont);
}

static void gameSetupMenu_setTexts(gameSetupMenu* menu)
{
    displayField_setTextString(menu->titlebar, "GAME SETUP");

    displayField_setTextString(
        menu->info,
        "Choose the type of game session to create.\n"
        "The selected action is reported upward."
    );

    button_setTextString(menu->localPvP, "Local PvP");
    button_setTextString(menu->vsEngine, "Vs Engine");
    button_setTextString(menu->networkPvP, "Network PvP");
    button_setTextString(menu->back, "Back");
}

static void gameSetupMenu_setStyle(gameSetupMenu* menu)
{
    displayField_setFillColor(menu->titlebar, sfWhite);
    displayField_setTextColor(menu->titlebar, sfWhite);
    displayField_setCharacterSize(menu->titlebar, 56);
    displayField_setLetterSpacing(menu->titlebar, 2.0f);
    displayField_setTextPadding(menu->titlebar, (sfVector2f){0.0f, 0.0f});
    displayField_setTextAlignment(
        menu->titlebar,
        displayFieldTextAlignCenter,
        displayFieldTextAlignMiddle
    );
    displayField_setOutlineThickness(menu->titlebar, 0.0f);

    displayField_setFillColor(menu->info, (sfColor){20, 20, 20, 180});
    displayField_setTextColor(menu->info, sfWhite);
    displayField_setCharacterSize(menu->info, 20);
    displayField_setLetterSpacing(menu->info, 1.0f);
    displayField_setTextPadding(menu->info, (sfVector2f){24.0f, 18.0f});
    displayField_setTextAlignment(
        menu->info,
        displayFieldTextAlignLeft,
        displayFieldTextAlignTop
    );
    displayField_setOutlineColor(menu->info, sfWhite);
    displayField_setOutlineThickness(menu->info, 1.0f);

    button_setFillColor(menu->localPvP, sfWhite);
    button_setFillColor(menu->vsEngine, sfWhite);
    button_setFillColor(menu->networkPvP, sfWhite);
    button_setFillColor(menu->back, sfWhite);

    button_setOutlineThickness(menu->localPvP, 0.0f);
    button_setOutlineThickness(menu->vsEngine, 0.0f);
    button_setOutlineThickness(menu->networkPvP, 0.0f);
    button_setOutlineThickness(menu->back, 0.0f);
}

static void gameSetupMenu_setLayout(gameSetupMenu* menu, sfVector2i topLeft, sfVector2i bottomRight)
{
    float areaWidth;
    float areaHeight;
    float centerX;

    sfVector2f titleSize;
    sfVector2f infoSize;
    sfVector2f buttonSize;

    float titleY;
    float infoY;
    float firstButtonY;
    float buttonGap;

    areaWidth = (float)(bottomRight.x - topLeft.x);
    areaHeight = (float)(bottomRight.y - topLeft.y);

    centerX = (float)topLeft.x + areaWidth / 2.0f;

    titleSize = (sfVector2f){areaWidth * 0.62f, areaHeight * 0.15f};
    infoSize = (sfVector2f){areaWidth * 0.50f, areaHeight * 0.14f};
    buttonSize = (sfVector2f){areaWidth * 0.34f, areaHeight * 0.085f};

    titleY = (float)topLeft.y + areaHeight * 0.15f;
    infoY = (float)topLeft.y + areaHeight * 0.31f;
    firstButtonY = (float)topLeft.y + areaHeight * 0.50f;
    buttonGap = areaHeight * 0.11f;

    displayField_setSize(menu->titlebar, titleSize);
    displayField_setOrigin(menu->titlebar, (sfVector2f){titleSize.x / 2.0f, titleSize.y / 2.0f});
    displayField_setPosition(menu->titlebar, (sfVector2f){centerX, titleY});

    displayField_setSize(menu->info, infoSize);
    displayField_setOrigin(menu->info, (sfVector2f){infoSize.x / 2.0f, infoSize.y / 2.0f});
    displayField_setPosition(menu->info, (sfVector2f){centerX, infoY});

    button_setSize(menu->localPvP, buttonSize);
    button_setOrigin(menu->localPvP, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->localPvP, (sfVector2f){centerX, firstButtonY});

    button_setSize(menu->vsEngine, buttonSize);
    button_setOrigin(menu->vsEngine, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->vsEngine, (sfVector2f){centerX, firstButtonY + buttonGap});

    button_setSize(menu->networkPvP, buttonSize);
    button_setOrigin(menu->networkPvP, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->networkPvP, (sfVector2f){centerX, firstButtonY + 2.0f * buttonGap});

    button_setSize(menu->back, buttonSize);
    button_setOrigin(menu->back, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->back, (sfVector2f){centerX, firstButtonY + 3.0f * buttonGap});
}

static void gameSetupMenu_setActions(gameSetupMenu* menu)
{
    button_setAction(menu->localPvP, gameSetupMenu_onLocalPvP, menu);
    button_setAction(menu->vsEngine, gameSetupMenu_onVsEngine, menu);
    button_setAction(menu->networkPvP, gameSetupMenu_onNetworkPvP, menu);
    button_setAction(menu->back, gameSetupMenu_onBack, menu);
}

gameSetupMenu* gameSetupMenu_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    gameSetupMenu* menu;

    if (resources == NULL)
        return NULL;

    menu = malloc(sizeof(gameSetupMenu));

    if (menu == NULL)
        return NULL;

    menu->titlebar = NULL;
    menu->info = NULL;

    menu->localPvP = NULL;
    menu->vsEngine = NULL;
    menu->networkPvP = NULL;
    menu->back = NULL;

    menu->active = sfTrue;
    menu->action = gameSetupMenuActionNone;

    menu->titlebar = displayField_create();
    menu->info = displayField_create();

    menu->localPvP = button_create();
    menu->vsEngine = button_create();
    menu->networkPvP = button_create();
    menu->back = button_create();

    if (
        menu->titlebar == NULL ||
        menu->info == NULL ||
        menu->localPvP == NULL ||
        menu->vsEngine == NULL ||
        menu->networkPvP == NULL ||
        menu->back == NULL
    )
    {
        gameSetupMenu_destroy(menu);
        return NULL;
    }

    gameSetupMenu_applyResources(menu, resources);
    gameSetupMenu_setTexts(menu);
    gameSetupMenu_setStyle(menu);
    gameSetupMenu_setLayout(menu, topLeft, bottomRight);
    gameSetupMenu_setActions(menu);

    return menu;
}

void gameSetupMenu_destroy(gameSetupMenu* menu)
{
    if (menu == NULL)
        return;

    if (menu->titlebar != NULL)
        displayField_destroy(menu->titlebar);

    if (menu->info != NULL)
        displayField_destroy(menu->info);

    if (menu->localPvP != NULL)
        button_destroy(menu->localPvP);

    if (menu->vsEngine != NULL)
        button_destroy(menu->vsEngine);

    if (menu->networkPvP != NULL)
        button_destroy(menu->networkPvP);

    if (menu->back != NULL)
        button_destroy(menu->back);

    free(menu);
}

void gameSetupMenu_updateMouse(gameSetupMenu* menu, const Mouse* mouse)
{
    if (menu == NULL || mouse == NULL)
        return;

    if (!menu->active)
        return;

    button_updateMouse(menu->localPvP, mouse);
    button_updateMouse(menu->vsEngine, mouse);
    button_updateMouse(menu->networkPvP, mouse);
    button_updateMouse(menu->back, mouse);
}

void gameSetupMenu_setActive(gameSetupMenu* menu, sfBool active)
{
    if (menu == NULL)
        return;

    menu->active = active;
}

sfBool gameSetupMenu_isActive(const gameSetupMenu* menu)
{
    if (menu == NULL)
        return sfFalse;

    return menu->active;
}

GameSetupMenuAction gameSetupMenu_getAction(const gameSetupMenu* menu)
{
    if (menu == NULL)
        return gameSetupMenuActionNone;

    return menu->action;
}

GameSetupMenuAction gameSetupMenu_consumeAction(gameSetupMenu* menu)
{
    GameSetupMenuAction action;

    if (menu == NULL)
        return gameSetupMenuActionNone;

    action = menu->action;
    menu->action = gameSetupMenuActionNone;

    return action;
}

void gameSetupMenu_draw(sfRenderWindow* window, const gameSetupMenu* menu)
{
    if (window == NULL || menu == NULL)
        return;

    if (!menu->active)
        return;

    displayField_draw(window, menu->titlebar);
    displayField_draw(window, menu->info);

    button_draw(window, menu->localPvP);
    button_draw(window, menu->vsEngine);
    button_draw(window, menu->networkPvP);
    button_draw(window, menu->back);
}