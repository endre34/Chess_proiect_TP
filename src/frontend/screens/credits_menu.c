#include "frontend/screens/credits_menu.h"

#include "frontend/ui/button.h"
#include "frontend/ui/display_field.h"

#include <stdlib.h>

#define MENU_BUTTON_TEXTURE_WIDTH 916
#define MENU_BUTTON_TEXTURE_HEIGHT 240

#define MENU_TITLE_TEXTURE_WIDTH 1600
#define MENU_TITLE_TEXTURE_HEIGHT 320

struct creditsMenu
{
    DisplayField* titlebar;
    DisplayField* credits;

    Button* back;

    sfBool active;

    CreditsMenuAction action;
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

static void creditsMenu_onBack(void*);

static void creditsMenu_applyResources(creditsMenu*, const Resources*);
static void creditsMenu_setTexts(creditsMenu*);
static void creditsMenu_setStyle(creditsMenu*);
static void creditsMenu_setLayout(creditsMenu*, sfVector2i, sfVector2i);
static void creditsMenu_setActions(creditsMenu*);

static void creditsMenu_setupButtonTexture(Button*, const sfTexture*);
static void creditsMenu_setupButtonText(Button*, const sfFont*);

static void creditsMenu_onBack(void* data)
{
    ((creditsMenu*)data)->action = creditsMenuActionBack;
}

static void creditsMenu_setupButtonTexture(Button* button, const sfTexture* texture)
{
    button_setTexture(button, texture, sfFalse);

    button_setTextureRect_onIdle(button, MENU_BUTTON_IDLE_RECT);
    button_setTextureRect_onHover(button, MENU_BUTTON_HOVER_RECT);
    button_setTextureRect_onPress(button, MENU_BUTTON_PRESS_RECT);
}

static void creditsMenu_setupButtonText(Button* button, const sfFont* font)
{
    button_setTextFont(button, font);
    button_setTextColor(button, sfWhite);
    button_setCharacterSize(button, 32);
    button_setLetterSpacing(button, 1.3f);
}

static void creditsMenu_applyResources(creditsMenu* menu, const Resources* resources)
{
    const sfTexture* buttonTexture;
    const sfTexture* titleBoardTexture;

    const sfFont* titleFont;
    const sfFont* buttonFont;
    const sfFont* creditsFont;

    buttonTexture = resources_getTexture(resources, resourceTextureButtons);
    titleBoardTexture = resources_getTexture(resources, resourceTextureTitleBoard);

    titleFont = resources_getFont(resources, resourceFontCinzelSemiBold);
    buttonFont = resources_getFont(resources, resourceFontCinzelMedium);
    creditsFont = resources_getFont(resources, resourceFontJetBrainsMonoRegular);

    displayField_setTexture(menu->titlebar, titleBoardTexture, sfFalse);
    displayField_setTextureRect(menu->titlebar, MENU_TITLE_RECT);
    displayField_setTextFont(menu->titlebar, titleFont);

    displayField_setTextFont(menu->credits, creditsFont);

    creditsMenu_setupButtonTexture(menu->back, buttonTexture);
    creditsMenu_setupButtonText(menu->back, buttonFont);
}

static void creditsMenu_setTexts(creditsMenu* menu)
{
    displayField_setTextString(menu->titlebar, "CREDITS");

    displayField_setTextString(
        menu->credits,
        "TP Chess Project\n\n"
        "Programming and UI architecture:\n"
        "  Student project implementation\n\n"
        "Graphics and windowing:\n"
        "  CSFML / SFML\n\n"
        "Future engine support:\n"
        "  Stockfish\n\n"
        "Assets:\n"
        "  Project media folder resources\n\n"
        "Note:\n"
        "  Exact third-party license text should be added\n"
        "  before final distribution."
    );

    button_setTextString(menu->back, "Back");
}

static void creditsMenu_setStyle(creditsMenu* menu)
{
    displayField_setFillColor(menu->titlebar, sfWhite);
    displayField_setTextColor(menu->titlebar, sfWhite);
    displayField_setCharacterSize(menu->titlebar, 58);
    displayField_setLetterSpacing(menu->titlebar, 2.0f);
    displayField_setTextPadding(menu->titlebar, (sfVector2f){0.0f, 0.0f});
    displayField_setTextAlignment(
        menu->titlebar,
        displayFieldTextAlignCenter,
        displayFieldTextAlignMiddle
    );
    displayField_setOutlineThickness(menu->titlebar, 0.0f);

    displayField_setFillColor(menu->credits, (sfColor){20, 20, 20, 190});
    displayField_setTextColor(menu->credits, sfWhite);
    displayField_setCharacterSize(menu->credits, 21);
    displayField_setLetterSpacing(menu->credits, 1.0f);
    displayField_setTextPadding(menu->credits, (sfVector2f){28.0f, 24.0f});
    displayField_setTextAlignment(
        menu->credits,
        displayFieldTextAlignLeft,
        displayFieldTextAlignTop
    );
    displayField_setOutlineColor(menu->credits, sfWhite);
    displayField_setOutlineThickness(menu->credits, 1.0f);

    button_setFillColor(menu->back, sfWhite);
    button_setOutlineThickness(menu->back, 0.0f);
}

static void creditsMenu_setLayout(creditsMenu* menu, sfVector2i topLeft, sfVector2i bottomRight)
{
    float areaWidth;
    float areaHeight;
    float centerX;

    sfVector2f titleSize;
    sfVector2f creditsSize;
    sfVector2f buttonSize;

    float titleY;
    float creditsY;
    float backY;

    areaWidth = (float)(bottomRight.x - topLeft.x);
    areaHeight = (float)(bottomRight.y - topLeft.y);

    centerX = (float)topLeft.x + areaWidth / 2.0f;

    titleSize = (sfVector2f){areaWidth * 0.58f, areaHeight * 0.15f};
    creditsSize = (sfVector2f){areaWidth * 0.56f, areaHeight * 0.48f};
    buttonSize = (sfVector2f){areaWidth * 0.26f, areaHeight * 0.085f};

    titleY = (float)topLeft.y + areaHeight * 0.15f;
    creditsY = (float)topLeft.y + areaHeight * 0.46f;
    backY = (float)topLeft.y + areaHeight * 0.80f;

    displayField_setSize(menu->titlebar, titleSize);
    displayField_setOrigin(menu->titlebar, (sfVector2f){titleSize.x / 2.0f, titleSize.y / 2.0f});
    displayField_setPosition(menu->titlebar, (sfVector2f){centerX, titleY});

    displayField_setSize(menu->credits, creditsSize);
    displayField_setOrigin(menu->credits, (sfVector2f){creditsSize.x / 2.0f, creditsSize.y / 2.0f});
    displayField_setPosition(menu->credits, (sfVector2f){centerX, creditsY});

    button_setSize(menu->back, buttonSize);
    button_setOrigin(menu->back, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->back, (sfVector2f){centerX, backY});
}

static void creditsMenu_setActions(creditsMenu* menu)
{
    button_setAction(menu->back, creditsMenu_onBack, menu);
}

creditsMenu* creditsMenu_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    creditsMenu* menu;

    if (resources == NULL)
        return NULL;

    menu = malloc(sizeof(creditsMenu));

    if (menu == NULL)
        return NULL;

    menu->titlebar = NULL;
    menu->credits = NULL;

    menu->back = NULL;

    menu->active = sfTrue;
    menu->action = creditsMenuActionNone;

    menu->titlebar = displayField_create();
    menu->credits = displayField_create();

    menu->back = button_create();

    if (
        menu->titlebar == NULL ||
        menu->credits == NULL ||
        menu->back == NULL
    )
    {
        creditsMenu_destroy(menu);
        return NULL;
    }

    creditsMenu_applyResources(menu, resources);
    creditsMenu_setTexts(menu);
    creditsMenu_setStyle(menu);
    creditsMenu_setLayout(menu, topLeft, bottomRight);
    creditsMenu_setActions(menu);

    return menu;
}

void creditsMenu_destroy(creditsMenu* menu)
{
    if (menu == NULL)
        return;

    if (menu->titlebar != NULL)
        displayField_destroy(menu->titlebar);

    if (menu->credits != NULL)
        displayField_destroy(menu->credits);

    if (menu->back != NULL)
        button_destroy(menu->back);

    free(menu);
}

void creditsMenu_updateMouse(creditsMenu* menu, const Mouse* mouse)
{
    if (menu == NULL || mouse == NULL)
        return;

    if (!menu->active)
        return;

    button_updateMouse(menu->back, mouse);
}

void creditsMenu_setActive(creditsMenu* menu, sfBool active)
{
    if (menu == NULL)
        return;

    menu->active = active;
}

sfBool creditsMenu_isActive(const creditsMenu* menu)
{
    if (menu == NULL)
        return sfFalse;

    return menu->active;
}

CreditsMenuAction creditsMenu_getAction(const creditsMenu* menu)
{
    if (menu == NULL)
        return creditsMenuActionNone;

    return menu->action;
}

CreditsMenuAction creditsMenu_consumeAction(creditsMenu* menu)
{
    CreditsMenuAction action;

    if (menu == NULL)
        return creditsMenuActionNone;

    action = menu->action;
    menu->action = creditsMenuActionNone;

    return action;
}

void creditsMenu_draw(sfRenderWindow* window, const creditsMenu* menu)
{
    if (window == NULL || menu == NULL)
        return;

    if (!menu->active)
        return;

    displayField_draw(window, menu->titlebar);
    displayField_draw(window, menu->credits);

    button_draw(window, menu->back);
}