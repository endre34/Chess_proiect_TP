#include "frontend/screens/settings_menu.h"

#include "frontend/ui/button.h"
#include "frontend/ui/display_field.h"

#include <stdlib.h>

#define MENU_BUTTON_TEXTURE_WIDTH 894
#define MENU_BUTTON_TEXTURE_HEIGHT 234

#define MENU_TITLE_TEXTURE_WIDTH 1596
#define MENU_TITLE_TEXTURE_HEIGHT 316

struct settingsMenu
{
    DisplayField* titlebar;

    Button* sound;
    Button* fullscreen;
    Button* controls;
    Button* back;

    SettingsData data;

    sfBool active;

    SettingsMenuAction action;
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

static void settingsMenu_onSound(void*);
static void settingsMenu_onFullscreen(void*);
static void settingsMenu_onControls(void*);
static void settingsMenu_onBack(void*);

static void settingsMenu_applyResources(settingsMenu*, const Resources*);
static void settingsMenu_setTexts(settingsMenu*);
static void settingsMenu_updateTexts(settingsMenu*);
static void settingsMenu_setStyle(settingsMenu*);
static void settingsMenu_setLayout(settingsMenu*, sfVector2i, sfVector2i);
static void settingsMenu_setActions(settingsMenu*);

static void settingsMenu_setupButtonTexture(Button*, const sfTexture*);
static void settingsMenu_setupButtonText(Button*, const sfFont*);

static void settingsMenu_onSound(void* data)
{
    settingsMenu* menu;

    menu = data;

    menu->data.soundEnabled = !menu->data.soundEnabled;
    menu->action = settingsMenuActionApply;

    settingsMenu_updateTexts(menu);
}

static void settingsMenu_onFullscreen(void* data)
{
    settingsMenu* menu;

    menu = data;

    menu->data.fullscreenEnabled = !menu->data.fullscreenEnabled;
    menu->action = settingsMenuActionApply;

    settingsMenu_updateTexts(menu);
}

static void settingsMenu_onControls(void* data)
{
    ((settingsMenu*)data)->action = settingsMenuActionControls;
}

static void settingsMenu_onBack(void* data)
{
    ((settingsMenu*)data)->action = settingsMenuActionBack;
}

static void settingsMenu_setupButtonTexture(Button* button, const sfTexture* texture)
{
    button_setTexture(button, texture, sfFalse);

    button_setTextureRect_onIdle(button, MENU_BUTTON_IDLE_RECT);
    button_setTextureRect_onHover(button, MENU_BUTTON_HOVER_RECT);
    button_setTextureRect_onPress(button, MENU_BUTTON_PRESS_RECT);
}

static void settingsMenu_setupButtonText(Button* button, const sfFont* font)
{
    button_setTextFont(button, font);
    button_setTextColor(button, sfWhite);
    button_setCharacterSize(button, 32);
    button_setLetterSpacing(button, 1.3f);
}

static void settingsMenu_applyResources(settingsMenu* menu, const Resources* resources)
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

    settingsMenu_setupButtonTexture(menu->sound, buttonTexture);
    settingsMenu_setupButtonTexture(menu->fullscreen, buttonTexture);
    settingsMenu_setupButtonTexture(menu->controls, buttonTexture);
    settingsMenu_setupButtonTexture(menu->back, buttonTexture);

    settingsMenu_setupButtonText(menu->sound, buttonFont);
    settingsMenu_setupButtonText(menu->fullscreen, buttonFont);
    settingsMenu_setupButtonText(menu->controls, buttonFont);
    settingsMenu_setupButtonText(menu->back, buttonFont);
}

static void settingsMenu_setTexts(settingsMenu* menu)
{
    displayField_setTextString(menu->titlebar, "SETTINGS");

    button_setTextString(menu->controls, "Controls");
    button_setTextString(menu->back, "Back");

    settingsMenu_updateTexts(menu);
}

static void settingsMenu_updateTexts(settingsMenu* menu)
{
    if (menu->data.soundEnabled)
        button_setTextString(menu->sound, "Sound: ON");
    else
        button_setTextString(menu->sound, "Sound: OFF");

    if (menu->data.fullscreenEnabled)
        button_setTextString(menu->fullscreen, "Fullscreen: ON");
    else
        button_setTextString(menu->fullscreen, "Fullscreen: OFF");
}

static void settingsMenu_setStyle(settingsMenu* menu)
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

    button_setFillColor(menu->sound, sfWhite);
    button_setFillColor(menu->fullscreen, sfWhite);
    button_setFillColor(menu->controls, sfWhite);
    button_setFillColor(menu->back, sfWhite);

    button_setOutlineThickness(menu->sound, 0.0f);
    button_setOutlineThickness(menu->fullscreen, 0.0f);
    button_setOutlineThickness(menu->controls, 0.0f);
    button_setOutlineThickness(menu->back, 0.0f);
}

static void settingsMenu_setLayout(settingsMenu* menu, sfVector2i topLeft, sfVector2i bottomRight)
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

    titleSize = (sfVector2f){areaWidth * 0.58f, areaHeight * 0.15f};
    buttonSize = (sfVector2f){areaWidth * 0.32f, areaHeight * 0.085f};

    titleY = (float)topLeft.y + areaHeight * 0.17f;
    firstButtonY = (float)topLeft.y + areaHeight * 0.40f;
    buttonGap = areaHeight * 0.11f;

    displayField_setSize(menu->titlebar, titleSize);
    displayField_setOrigin(menu->titlebar, (sfVector2f){titleSize.x / 2.0f, titleSize.y / 2.0f});
    displayField_setPosition(menu->titlebar, (sfVector2f){centerX, titleY});

    button_setSize(menu->sound, buttonSize);
    button_setOrigin(menu->sound, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->sound, (sfVector2f){centerX, firstButtonY});

    button_setSize(menu->fullscreen, buttonSize);
    button_setOrigin(menu->fullscreen, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->fullscreen, (sfVector2f){centerX, firstButtonY + buttonGap});

    button_setSize(menu->controls, buttonSize);
    button_setOrigin(menu->controls, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->controls, (sfVector2f){centerX, firstButtonY + 2.0f * buttonGap});

    button_setSize(menu->back, buttonSize);
    button_setOrigin(menu->back, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->back, (sfVector2f){centerX, firstButtonY + 3.0f * buttonGap});
}

static void settingsMenu_setActions(settingsMenu* menu)
{
    button_setAction(menu->sound, settingsMenu_onSound, menu);
    button_setAction(menu->fullscreen, settingsMenu_onFullscreen, menu);
    button_setAction(menu->controls, settingsMenu_onControls, menu);
    button_setAction(menu->back, settingsMenu_onBack, menu);
}

settingsMenu* settingsMenu_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    settingsMenu* menu;

    if (resources == NULL)
        return NULL;

    menu = malloc(sizeof(settingsMenu));

    if (menu == NULL)
        return NULL;

    menu->titlebar = NULL;

    menu->sound = NULL;
    menu->fullscreen = NULL;
    menu->controls = NULL;
    menu->back = NULL;

    menu->data = settingsData_getDefault();

    menu->active = sfTrue;
    menu->action = settingsMenuActionNone;

    menu->titlebar = displayField_create();

    menu->sound = button_create();
    menu->fullscreen = button_create();
    menu->controls = button_create();
    menu->back = button_create();

    if (
        menu->titlebar == NULL ||
        menu->sound == NULL ||
        menu->fullscreen == NULL ||
        menu->controls == NULL ||
        menu->back == NULL
    )
    {
        settingsMenu_destroy(menu);
        return NULL;
    }

    settingsMenu_applyResources(menu, resources);
    settingsMenu_setTexts(menu);
    settingsMenu_setStyle(menu);
    settingsMenu_setLayout(menu, topLeft, bottomRight);
    settingsMenu_setActions(menu);

    return menu;
}

void settingsMenu_destroy(settingsMenu* menu)
{
    if (menu == NULL)
        return;

    if (menu->titlebar != NULL)
        displayField_destroy(menu->titlebar);

    if (menu->sound != NULL)
        button_destroy(menu->sound);

    if (menu->fullscreen != NULL)
        button_destroy(menu->fullscreen);

    if (menu->controls != NULL)
        button_destroy(menu->controls);

    if (menu->back != NULL)
        button_destroy(menu->back);

    free(menu);
}

void settingsMenu_updateMouse(settingsMenu* menu, const Mouse* mouse)
{
    if (menu == NULL || mouse == NULL)
        return;

    if (!menu->active)
        return;

    button_updateMouse(menu->sound, mouse);
    button_updateMouse(menu->fullscreen, mouse);
    button_updateMouse(menu->controls, mouse);
    button_updateMouse(menu->back, mouse);
}

void settingsMenu_setActive(settingsMenu* menu, sfBool active)
{
    if (menu == NULL)
        return;

    menu->active = active;
}

sfBool settingsMenu_isActive(const settingsMenu* menu)
{
    if (menu == NULL)
        return sfFalse;

    return menu->active;
}

SettingsMenuAction settingsMenu_getAction(const settingsMenu* menu)
{
    if (menu == NULL)
        return settingsMenuActionNone;

    return menu->action;
}

SettingsMenuAction settingsMenu_consumeAction(settingsMenu* menu)
{
    SettingsMenuAction action;

    if (menu == NULL)
        return settingsMenuActionNone;

    action = menu->action;
    menu->action = settingsMenuActionNone;

    return action;
}

SettingsData settingsMenu_getData(const settingsMenu* menu)
{
    if (menu == NULL)
        return settingsData_getDefault();

    return menu->data;
}

void settingsMenu_draw(sfRenderWindow* window, const settingsMenu* menu)
{
    if (window == NULL || menu == NULL)
        return;

    if (!menu->active)
        return;

    displayField_draw(window, menu->titlebar);

    button_draw(window, menu->sound);
    button_draw(window, menu->fullscreen);
    button_draw(window, menu->controls);
    button_draw(window, menu->back);
}