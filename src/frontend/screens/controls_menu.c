#include "frontend/screens/controls_menu.h"

#include "frontend/ui/button.h"
#include "frontend/ui/display_field.h"

#include <stdlib.h>

#define MENU_BUTTON_TEXTURE_WIDTH 894
#define MENU_BUTTON_TEXTURE_HEIGHT 234

#define MENU_TITLE_TEXTURE_WIDTH 1596
#define MENU_TITLE_TEXTURE_HEIGHT 316

struct controlsMenu
{
    DisplayField* titlebar;

    Button* back;

    sfBool active;

    ControlsMenuAction action;
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

static void controlsMenu_onBack(void*);

static void controlsMenu_applyResources(controlsMenu*, const Resources*);
static void controlsMenu_setTexts(controlsMenu*);
static void controlsMenu_setStyle(controlsMenu*);
static void controlsMenu_setLayout(controlsMenu*, sfVector2i, sfVector2i);
static void controlsMenu_setActions(controlsMenu*);

static void controlsMenu_setupButtonTexture(Button*, const sfTexture*);
static void controlsMenu_setupButtonText(Button*, const sfFont*);

static void controlsMenu_onBack(void* data)
{
    ((controlsMenu*)data)->action = controlsMenuActionBack;
}

static void controlsMenu_setupButtonTexture(Button* button, const sfTexture* texture)
{
    button_setTexture(button, texture, sfFalse);

    button_setTextureRect_onIdle(button, MENU_BUTTON_IDLE_RECT);
    button_setTextureRect_onHover(button, MENU_BUTTON_HOVER_RECT);
    button_setTextureRect_onPress(button, MENU_BUTTON_PRESS_RECT);
}

static void controlsMenu_setupButtonText(Button* button, const sfFont* font)
{
    button_setTextFont(button, font);
    button_setTextColor(button, sfWhite);
    button_setCharacterSize(button, 32);
    button_setLetterSpacing(button, 1.3f);
}

static void controlsMenu_applyResources(controlsMenu* menu, const Resources* resources)
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
    displayField_setTextureRect(menu->titlebar, MENU_TITLE_RECT);
    displayField_setTextFont(menu->titlebar, titleFont);

    controlsMenu_setupButtonTexture(menu->back, buttonTexture);
    controlsMenu_setupButtonText(menu->back, buttonFont);
}

static void controlsMenu_setTexts(controlsMenu* menu)
{
    displayField_setTextString(menu->titlebar, "CONTROLS");

    button_setTextString(menu->back, "Back");
}

static void controlsMenu_setStyle(controlsMenu* menu)
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

    button_setFillColor(menu->back, sfWhite);
    button_setOutlineThickness(menu->back, 0.0f);
}

static void controlsMenu_setLayout(controlsMenu* menu, sfVector2i topLeft, sfVector2i bottomRight)
{
    float areaWidth;
    float areaHeight;
    float centerX;

    sfVector2f titleSize;
    sfVector2f buttonSize;

    float titleY;
    float backY;

    areaWidth = (float)(bottomRight.x - topLeft.x);
    areaHeight = (float)(bottomRight.y - topLeft.y);

    centerX = (float)topLeft.x + areaWidth / 2.0f;

    titleSize = (sfVector2f){areaWidth * 0.58f, areaHeight * 0.15f};
    buttonSize = (sfVector2f){areaWidth * 0.26f, areaHeight * 0.085f};

    titleY = (float)topLeft.y + areaHeight * 0.17f;
    backY = (float)topLeft.y + areaHeight * 0.50f;

    displayField_setSize(menu->titlebar, titleSize);
    displayField_setOrigin(menu->titlebar, (sfVector2f){titleSize.x / 2.0f, titleSize.y / 2.0f});
    displayField_setPosition(menu->titlebar, (sfVector2f){centerX, titleY});

    button_setSize(menu->back, buttonSize);
    button_setOrigin(menu->back, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->back, (sfVector2f){centerX, backY});
}

static void controlsMenu_setActions(controlsMenu* menu)
{
    button_setAction(menu->back, controlsMenu_onBack, menu);
}

controlsMenu* controlsMenu_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    controlsMenu* menu;

    if (resources == NULL)
        return NULL;

    menu = malloc(sizeof(controlsMenu));

    if (menu == NULL)
        return NULL;

    menu->titlebar = NULL;
    menu->back = NULL;

    menu->active = sfTrue;
    menu->action = controlsMenuActionNone;

    menu->titlebar = displayField_create();
    menu->back = button_create();

    if (
        menu->titlebar == NULL ||
        menu->back == NULL
    )
    {
        controlsMenu_destroy(menu);
        return NULL;
    }

    controlsMenu_applyResources(menu, resources);
    controlsMenu_setTexts(menu);
    controlsMenu_setStyle(menu);
    controlsMenu_setLayout(menu, topLeft, bottomRight);
    controlsMenu_setActions(menu);

    return menu;
}

void controlsMenu_destroy(controlsMenu* menu)
{
    if (menu == NULL)
        return;

    if (menu->titlebar != NULL)
        displayField_destroy(menu->titlebar);

    if (menu->back != NULL)
        button_destroy(menu->back);

    free(menu);
}

void controlsMenu_updateMouse(controlsMenu* menu, const Mouse* mouse)
{
    if (menu == NULL || mouse == NULL)
        return;

    if (!menu->active)
        return;

    button_updateMouse(menu->back, mouse);
}

void controlsMenu_setActive(controlsMenu* menu, sfBool active)
{
    if (menu == NULL)
        return;

    menu->active = active;
}

sfBool controlsMenu_isActive(const controlsMenu* menu)
{
    if (menu == NULL)
        return sfFalse;

    return menu->active;
}

ControlsMenuAction controlsMenu_getAction(const controlsMenu* menu)
{
    if (menu == NULL)
        return controlsMenuActionNone;

    return menu->action;
}

ControlsMenuAction controlsMenu_consumeAction(controlsMenu* menu)
{
    ControlsMenuAction action;

    if (menu == NULL)
        return controlsMenuActionNone;

    action = menu->action;
    menu->action = controlsMenuActionNone;

    return action;
}

void controlsMenu_draw(sfRenderWindow* window, const controlsMenu* menu)
{
    if (window == NULL || menu == NULL)
        return;

    if (!menu->active)
        return;

    displayField_draw(window, menu->titlebar);

    button_draw(window, menu->back);
}