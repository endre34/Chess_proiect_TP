#include "main_menu.h"

#include "button.h"
#include "display_field.h"

#include <stdlib.h>

#define MAIN_MENU_BUTTON_TEXTURE_WIDTH 894
#define MAIN_MENU_BUTTON_TEXTURE_HEIGHT 234

#define MAIN_MENU_TITLE_TEXTURE_WIDTH 1596
#define MAIN_MENU_TITLE_TEXTURE_HEIGHT 316

struct mainMenu
{
    DisplayField* titlebar;

    Button* play;
    Button* settings;
    Button* credits;
    Button* exit;

    sfBool active;

    MainMenuAction action;
};

static const sfIntRect MAIN_MENU_BUTTON_IDLE_RECT = 
{
    0,
    0,
    MAIN_MENU_BUTTON_TEXTURE_WIDTH,
    MAIN_MENU_BUTTON_TEXTURE_HEIGHT
};

static const sfIntRect MAIN_MENU_BUTTON_HOVER_RECT = 
{
    MAIN_MENU_BUTTON_TEXTURE_WIDTH,
    0,
    MAIN_MENU_BUTTON_TEXTURE_WIDTH,
    MAIN_MENU_BUTTON_TEXTURE_HEIGHT
};

static const sfIntRect MAIN_MENU_BUTTON_PRESS_RECT = 
{
    2 * MAIN_MENU_BUTTON_TEXTURE_WIDTH,
    0,
    MAIN_MENU_BUTTON_TEXTURE_WIDTH,
    MAIN_MENU_BUTTON_TEXTURE_HEIGHT
};

static const sfIntRect MAIN_MENU_TITLE_RECT = 
{
    0,
    0,
    MAIN_MENU_TITLE_TEXTURE_WIDTH,
    MAIN_MENU_TITLE_TEXTURE_HEIGHT
};

static void mainMenu_onPlay(void*);
static void mainMenu_onSettings(void*);
static void mainMenu_onCredits(void*);
static void mainMenu_onExit(void*);

static void mainMenu_applyResources(mainMenu*, const Resources*);
static void mainMenu_setTexts(mainMenu*);
static void mainMenu_setStyle(mainMenu*);
static void mainMenu_setLayout(mainMenu*, sfVector2i, sfVector2i);
static void mainMenu_setActions(mainMenu*);

static void mainMenu_setupButtonTexture(Button*, const sfTexture*);
static void mainMenu_setupButtonText(Button*, const sfFont*);

static void mainMenu_onPlay(void* data)
{
    ((mainMenu*)data)->action = mainMenuActionPlay;
}

static void mainMenu_onSettings(void* data)
{
    ((mainMenu*)data)->action = mainMenuActionSettings;
}

static void mainMenu_onCredits(void* data)
{
    ((mainMenu*)data)->action = mainMenuActionCredits;
}

static void mainMenu_onExit(void* data)
{
    ((mainMenu*)data)->action = mainMenuActionExit;
}

static void mainMenu_setupButtonTexture(Button* button, const sfTexture* texture)
{
    button_setTexture(button, texture, sfFalse);

    button_setTextureRect_onIdle(button, MAIN_MENU_BUTTON_IDLE_RECT);
    button_setTextureRect_onHover(button, MAIN_MENU_BUTTON_HOVER_RECT);
    button_setTextureRect_onPress(button, MAIN_MENU_BUTTON_PRESS_RECT);
}

static void mainMenu_setupButtonText(Button* button, const sfFont* font)
{
    button_setTextFont(button, font);
    button_setTextColor(button, sfWhite);
    button_setCharacterSize(button, 34);
    button_setLetterSpacing(button, 1.4f);
}

static void mainMenu_applyResources(mainMenu* menu, const Resources* resources)
{
    const sfTexture* buttonTexture;
    const sfTexture* titleBoardTexture;

    const sfFont* titleFont;
    const sfFont* buttonFont;

    buttonTexture = resources_getTexture(resources, resourceTextureButtons);
    titleBoardTexture = resources_getTexture(resources, resourceTextureTitleBoard);

    titleFont = resources_getFont(resources, resourceFontCinzelSemiBold);
    buttonFont = resources_getFont(resources, resourceFontCinzelMedium);

    displayField_setTexture(menu->titlebar, titleBoardTexture, sfFalse);
    displayField_setTextureRect(menu->titlebar, MAIN_MENU_TITLE_RECT);
    displayField_setTextFont(menu->titlebar, titleFont);

    mainMenu_setupButtonTexture(menu->play, buttonTexture);
    mainMenu_setupButtonTexture(menu->settings, buttonTexture);
    mainMenu_setupButtonTexture(menu->credits, buttonTexture);
    mainMenu_setupButtonTexture(menu->exit, buttonTexture);

    mainMenu_setupButtonText(menu->play, buttonFont);
    mainMenu_setupButtonText(menu->settings, buttonFont);
    mainMenu_setupButtonText(menu->credits, buttonFont);
    mainMenu_setupButtonText(menu->exit, buttonFont);
}

static void mainMenu_setTexts(mainMenu* menu)
{
    displayField_setTextString(menu->titlebar, "CHESS");

    button_setTextString(menu->play, "Play");
    button_setTextString(menu->settings, "Settings");
    button_setTextString(menu->credits, "Credits");
    button_setTextString(menu->exit, "Exit");
}

static void mainMenu_setStyle(mainMenu* menu)
{
    displayField_setFillColor(menu->titlebar, sfWhite);
    displayField_setTextColor(menu->titlebar, sfWhite);
    displayField_setCharacterSize(menu->titlebar, 64);
    displayField_setLetterSpacing(menu->titlebar, 2.0f);
    displayField_setTextPadding(menu->titlebar, (sfVector2f){0.0f, 0.0f});
    displayField_setTextAlignment(menu->titlebar, displayFieldTextAlignCenter, displayFieldTextAlignMiddle);
    displayField_setOutlineThickness(menu->titlebar, 0.0f);

    button_setFillColor(menu->play, sfWhite);
    button_setFillColor(menu->settings, sfWhite);
    button_setFillColor(menu->credits, sfWhite);
    button_setFillColor(menu->exit, sfWhite);

    button_setOutlineThickness(menu->play, 0.0f);
    button_setOutlineThickness(menu->settings, 0.0f);
    button_setOutlineThickness(menu->credits, 0.0f);
    button_setOutlineThickness(menu->exit, 0.0f);
}

static void mainMenu_setLayout(mainMenu* menu, sfVector2i topLeft, sfVector2i bottomRight)
{
    float areaWidth;
    float areaHeight;

    float centerX;

    sfVector2f titleSize;
    sfVector2f buttonSize;

    float titleY;
    float firstButtonY;
    float buttonGap;

    areaWidth = (float)(bottomRight.x - topLeft.x);
    areaHeight = (float)(bottomRight.y - topLeft.y);

    centerX = (float)topLeft.x + areaWidth / 2.0f;

    titleSize = (sfVector2f){areaWidth * 0.60f, areaHeight * 0.16f};

    buttonSize = (sfVector2f){areaWidth * 0.34f, areaHeight * 0.09f};

    titleY = (float)topLeft.y + areaHeight * 0.16f;
    firstButtonY = (float)topLeft.y + areaHeight * 0.40f;
    buttonGap = areaHeight * 0.115f;

    displayField_setSize(menu->titlebar, titleSize);
    displayField_setOrigin(menu->titlebar, (sfVector2f){titleSize.x / 2.0f, titleSize.y / 2.0f});
    displayField_setPosition(menu->titlebar, (sfVector2f){centerX, titleY});

    button_setSize(menu->play, buttonSize);
    button_setOrigin(menu->play, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->play, (sfVector2f){centerX, firstButtonY});

    button_setSize(menu->settings, buttonSize);
    button_setOrigin(menu->settings, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->settings, (sfVector2f){centerX, firstButtonY + buttonGap});

    button_setSize(menu->credits, buttonSize);
    button_setOrigin(menu->credits, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->credits, (sfVector2f){centerX, firstButtonY + 2.0f * buttonGap});

    button_setSize(menu->exit, buttonSize);
    button_setOrigin(menu->exit, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->exit, (sfVector2f){centerX, firstButtonY + 3.0f * buttonGap});
}

static void mainMenu_setActions(mainMenu* menu)
{
    button_setAction(menu->play, mainMenu_onPlay, menu);
    button_setAction(menu->settings, mainMenu_onSettings, menu);
    button_setAction(menu->credits, mainMenu_onCredits, menu);
    button_setAction(menu->exit, mainMenu_onExit, menu);
}

mainMenu* mainMenu_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    mainMenu* menu;

    if (resources == NULL)
        return NULL;

    menu = malloc(sizeof(mainMenu));

    if (menu == NULL)
        return NULL;

    menu->titlebar = NULL;

    menu->play = NULL;
    menu->settings = NULL;
    menu->credits = NULL;
    menu->exit = NULL;

    menu->active = sfTrue;
    menu->action = mainMenuActionNone;

    menu->titlebar = displayField_create();

    menu->play = button_create();
    menu->settings = button_create();
    menu->credits = button_create();
    menu->exit = button_create();

    if (
        menu->titlebar == NULL ||
        menu->play == NULL ||
        menu->settings == NULL ||
        menu->credits == NULL ||
        menu->exit == NULL
    )
    {
        mainMenu_destroy(menu);
        return NULL;
    }

    mainMenu_applyResources(menu, resources);
    mainMenu_setTexts(menu);
    mainMenu_setStyle(menu);
    mainMenu_setLayout(menu, topLeft, bottomRight);
    mainMenu_setActions(menu);

    return menu;
}

void mainMenu_destroy(mainMenu* menu)
{
    if (menu == NULL)
        return;

    if (menu->titlebar != NULL)
        displayField_destroy(menu->titlebar);

    if (menu->play != NULL)
        button_destroy(menu->play);

    if (menu->settings != NULL)
        button_destroy(menu->settings);

    if (menu->credits != NULL)
        button_destroy(menu->credits);

    if (menu->exit != NULL)
        button_destroy(menu->exit);

    free(menu);
}

void mainMenu_updateMouse(mainMenu* menu, const Mouse* mouse)
{
    if (menu == NULL || mouse == NULL)
        return;

    if (!menu->active)
        return;

    button_updateMouse(menu->play, mouse);
    button_updateMouse(menu->settings, mouse);
    button_updateMouse(menu->credits, mouse);
    button_updateMouse(menu->exit, mouse);
}

void mainMenu_setActive(mainMenu* menu, sfBool active)
{
    if (menu == NULL)
        return;

    menu->active = active;
}

sfBool mainMenu_isActive(const mainMenu* menu)
{
    if (menu == NULL)
        return sfFalse;

    return menu->active;
}

MainMenuAction mainMenu_getAction(const mainMenu* menu)
{
    if (menu == NULL)
        return mainMenuActionNone;

    return menu->action;
}

MainMenuAction mainMenu_consumeAction(mainMenu* menu)
{
    MainMenuAction action;

    if (menu == NULL)
        return mainMenuActionNone;

    action = menu->action;
    menu->action = mainMenuActionNone;

    return action;
}

void mainMenu_draw(sfRenderWindow* window, const mainMenu* menu)
{
    if (window == NULL || menu == NULL)
        return;

    if (!menu->active)
        return;

    displayField_draw(window, menu->titlebar);

    button_draw(window, menu->play);
    button_draw(window, menu->settings);
    button_draw(window, menu->credits);
    button_draw(window, menu->exit);
}