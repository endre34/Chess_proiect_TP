#include "frontend/screens/engine_pve_setup_menu.h"

#include "frontend/ui/button.h"
#include "frontend/ui/display_field.h"
#include "frontend/ui/text_field.h"

#include <stdlib.h>

#define MENU_BUTTON_TEXTURE_WIDTH 894
#define MENU_BUTTON_TEXTURE_HEIGHT 234

#define MENU_TEXT_FIELD_TEXTURE_WIDTH 1790
#define MENU_TEXT_FIELD_TEXTURE_HEIGHT 470

#define MENU_TITLE_TEXTURE_WIDTH 1596
#define MENU_TITLE_TEXTURE_HEIGHT 316

struct enginePveSetupMenu
{
    DisplayField* titlebar;

    DisplayField* colorLabel;
    DisplayField* engineLevelLabel;
    DisplayField* noticeLabel;
    DisplayField* errorLabel;

    Button* color;
    TextField* engineLevelField;

    Button* play;
    Button* back;

    VsEngineSetup data;

    sfBool active;

    enginePveSetupMenuAction action;
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

static const sfIntRect MENU_TEXT_FIELD_IDLE_RECT = {
    0,
    0,
    MENU_TEXT_FIELD_TEXTURE_WIDTH,
    MENU_TEXT_FIELD_TEXTURE_HEIGHT
};

static const sfIntRect MENU_TEXT_FIELD_HOVER_RECT = {
    MENU_TEXT_FIELD_TEXTURE_WIDTH,
    0,
    MENU_TEXT_FIELD_TEXTURE_WIDTH,
    MENU_TEXT_FIELD_TEXTURE_HEIGHT
};

static const sfIntRect MENU_TEXT_FIELD_SELECTED_RECT = {
    2 * MENU_TEXT_FIELD_TEXTURE_WIDTH,
    0,
    MENU_TEXT_FIELD_TEXTURE_WIDTH,
    MENU_TEXT_FIELD_TEXTURE_HEIGHT
};

static const sfIntRect MENU_TITLE_RECT = {
    0,
    0,
    MENU_TITLE_TEXTURE_WIDTH,
    MENU_TITLE_TEXTURE_HEIGHT
};

static void enginePveSetupMenu_onColor(void*);
static void enginePveSetupMenu_onPlay(void*);
static void enginePveSetupMenu_onBack(void*);

static void enginePveSetupMenu_applyResources(enginePveSetupMenu*, const Resources*);
static void enginePveSetupMenu_setTexts(enginePveSetupMenu*);
static void enginePveSetupMenu_updateColorText(enginePveSetupMenu*);
static void enginePveSetupMenu_setStyle(enginePveSetupMenu*);
static void enginePveSetupMenu_setLayout(enginePveSetupMenu*, sfVector2i, sfVector2i);
static void enginePveSetupMenu_setActions(enginePveSetupMenu*);

static void enginePveSetupMenu_setupButtonTexture(Button*, const sfTexture*);
static void enginePveSetupMenu_setupButtonText(Button*, const sfFont*);
static void enginePveSetupMenu_setupTextField(TextField*, const sfTexture*, const sfFont*);
static void enginePveSetupMenu_setupPlainText(DisplayField*, const sfFont*, unsigned int, sfColor);

static sfBool enginePveSetupMenu_parseUnsigned(const char*, unsigned int*);
static sfBool enginePveSetupMenu_validate(enginePveSetupMenu*);

static void enginePveSetupMenu_onColor(void* data)
{
    enginePveSetupMenu* menu;

    menu = data;

    if (menu->data.playerColor == playerColorWhite)
        menu->data.playerColor = playerColorBlack;
    else if (menu->data.playerColor == playerColorBlack)
        menu->data.playerColor = playerColorRandom;
    else
        menu->data.playerColor = playerColorWhite;

    enginePveSetupMenu_updateColorText(menu);
}

static void enginePveSetupMenu_onPlay(void* data)
{
    enginePveSetupMenu* menu;

    menu = data;

    if (enginePveSetupMenu_validate(menu))
        menu->action = enginePveSetupMenuActionStart;
}

static void enginePveSetupMenu_onBack(void* data)
{
    ((enginePveSetupMenu*)data)->action = enginePveSetupMenuActionBack;
}

static void enginePveSetupMenu_setupButtonTexture(Button* button, const sfTexture* texture)
{
    button_setTexture(button, texture, sfFalse);

    button_setTextureRect_onIdle(button, MENU_BUTTON_IDLE_RECT);
    button_setTextureRect_onHover(button, MENU_BUTTON_HOVER_RECT);
    button_setTextureRect_onPress(button, MENU_BUTTON_PRESS_RECT);
}

static void enginePveSetupMenu_setupButtonText(Button* button, const sfFont* font)
{
    button_setTextFont(button, font);
    button_setTextColor(button, sfWhite);
    button_setCharacterSize(button, 30);
    button_setLetterSpacing(button, 1.2f);
}

static void enginePveSetupMenu_setupTextField(TextField* textField, const sfTexture* texture, const sfFont* font)
{
    textField_setTexture(textField, texture, sfFalse);

    textField_setTextureRect_onIdle(textField, MENU_TEXT_FIELD_IDLE_RECT);
    textField_setTextureRect_onHover(textField, MENU_TEXT_FIELD_HOVER_RECT);
    textField_setTextureRect_onSelected(textField, MENU_TEXT_FIELD_SELECTED_RECT);

    textField_setTextFont(textField, font);
    textField_setCharacterSize(textField, 32);
    textField_setLetterSpacing(textField, 1.0f);
    textField_setTextColor(textField, sfBlack);
    textField_setTextPadding(textField, (sfVector2f){24.0f, 0.0f});

    textField_setFillColor(textField, sfWhite);

    textField_setOutlineThickness(textField, 0.0f);
    textField_setSelectedOutlineThickness(textField, 0.0f);
}

static void enginePveSetupMenu_setupPlainText(DisplayField* field, const sfFont* font, unsigned int size, sfColor color)
{
    displayField_setTextFont(field, font);
    displayField_setTextColor(field, color);
    displayField_setCharacterSize(field, size);
    displayField_setLetterSpacing(field, 1.0f);
    displayField_setTextPadding(field, (sfVector2f){0.0f, 0.0f});
    displayField_setTextAlignment(
        field,
        displayFieldTextAlignCenter,
        displayFieldTextAlignMiddle
    );
    displayField_setFillColor(field, (sfColor){0, 0, 0, 0});
    displayField_setOutlineThickness(field, 0.0f);
}

static void enginePveSetupMenu_applyResources(enginePveSetupMenu* menu, const Resources* resources)
{
    const sfTexture* buttonTexture;
    const sfTexture* textFieldTexture;
    const sfTexture* titleBoardTexture;

    const sfFont* titleFont;
    const sfFont* buttonFont;
    const sfFont* plainFont;

    buttonTexture = resources_getTexture(resources, resourceTextureButtons);
    textFieldTexture = resources_getTexture(resources, resourceTextureTextFields);
    titleBoardTexture = resources_getTexture(resources, resourceTextureTitleBoard);

    titleFont = resources_getFont(resources, resourceFontCinzelSemiBold);
    buttonFont = resources_getFont(resources, resourceFontCinzelMedium);
    plainFont = resources_getFont(resources, resourceFontJetBrainsMonoRegular);

    displayField_setTexture(menu->titlebar, titleBoardTexture, sfFalse);
    displayField_setTextureRect(menu->titlebar, MENU_TITLE_RECT);
    displayField_setTextFont(menu->titlebar, titleFont);

    enginePveSetupMenu_setupPlainText(menu->colorLabel, titleFont, 32, sfWhite);
    enginePveSetupMenu_setupPlainText(menu->engineLevelLabel, titleFont, 32, sfWhite);
    enginePveSetupMenu_setupPlainText(menu->noticeLabel, plainFont, 18, sfWhite);
    enginePveSetupMenu_setupPlainText(menu->errorLabel, plainFont, 20, sfRed);

    enginePveSetupMenu_setupButtonTexture(menu->color, buttonTexture);
    enginePveSetupMenu_setupButtonTexture(menu->play, buttonTexture);
    enginePveSetupMenu_setupButtonTexture(menu->back, buttonTexture);

    enginePveSetupMenu_setupButtonText(menu->color, buttonFont);
    enginePveSetupMenu_setupButtonText(menu->play, buttonFont);
    enginePveSetupMenu_setupButtonText(menu->back, buttonFont);

    enginePveSetupMenu_setupTextField(menu->engineLevelField, textFieldTexture, plainFont);
}

static void enginePveSetupMenu_setTexts(enginePveSetupMenu* menu)
{
    displayField_setTextString(menu->titlebar, "VS ENGINE");
    displayField_setTextString(menu->colorLabel, "COLOR");
    displayField_setTextString(menu->engineLevelLabel, "ENGINE LEVEL");
    displayField_setTextString(menu->noticeLabel, "Only numbers accepted. Engine level must be in range 0-20.");
    displayField_setTextString(menu->errorLabel, "");

    textField_setTextString(menu->engineLevelField, "5");

    enginePveSetupMenu_updateColorText(menu);

    button_setTextString(menu->play, "Play");
    button_setTextString(menu->back, "Back");
}

static void enginePveSetupMenu_updateColorText(enginePveSetupMenu* menu)
{
    if (menu->data.playerColor == playerColorWhite)
        button_setTextString(menu->color, "White");
    else if (menu->data.playerColor == playerColorBlack)
        button_setTextString(menu->color, "Black");
    else
        button_setTextString(menu->color, "Random");
}

static void enginePveSetupMenu_setStyle(enginePveSetupMenu* menu)
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

    button_setFillColor(menu->color, sfWhite);
    button_setFillColor(menu->play, sfWhite);
    button_setFillColor(menu->back, sfWhite);

    button_setOutlineThickness(menu->color, 0.0f);
    button_setOutlineThickness(menu->play, 0.0f);
    button_setOutlineThickness(menu->back, 0.0f);
}

static void enginePveSetupMenu_setLayout(enginePveSetupMenu* menu, sfVector2i topLeft, sfVector2i bottomRight)
{
    float areaWidth;
    float areaHeight;
    float centerX;

    sfVector2f titleSize;
    sfVector2f labelSize;
    sfVector2f fieldSize;
    sfVector2f buttonSize;
    sfVector2f noticeSize;
    sfVector2f errorSize;

    float titleY;
    float colorLabelY;
    float colorY;
    float engineLabelY;
    float fieldY;
    float noticeY;
    float playY;
    float backY;
    float errorY;

    areaWidth = (float)(bottomRight.x - topLeft.x);
    areaHeight = (float)(bottomRight.y - topLeft.y);

    centerX = (float)topLeft.x + areaWidth / 2.0f;

    titleSize = (sfVector2f){areaWidth * 0.58f, areaHeight * 0.15f};
    labelSize = (sfVector2f){areaWidth * 0.34f, areaHeight * 0.06f};
    fieldSize = (sfVector2f){areaWidth * 0.16f, areaHeight * 0.075f};
    buttonSize = (sfVector2f){areaWidth * 0.28f, areaHeight * 0.085f};
    noticeSize = (sfVector2f){areaWidth * 0.70f, areaHeight * 0.05f};
    errorSize = (sfVector2f){areaWidth * 0.50f, areaHeight * 0.05f};

    titleY = (float)topLeft.y + areaHeight * 0.15f;
    colorLabelY = (float)topLeft.y + areaHeight * 0.31f;
    colorY = (float)topLeft.y + areaHeight * 0.39f;
    engineLabelY = (float)topLeft.y + areaHeight * 0.50f;
    fieldY = (float)topLeft.y + areaHeight * 0.58f;
    noticeY = (float)topLeft.y + areaHeight * 0.66f;
    playY = (float)topLeft.y + areaHeight * 0.76f;
    backY = (float)topLeft.y + areaHeight * 0.86f;
    errorY = (float)topLeft.y + areaHeight * 0.94f;

    displayField_setSize(menu->titlebar, titleSize);
    displayField_setOrigin(menu->titlebar, (sfVector2f){titleSize.x / 2.0f, titleSize.y / 2.0f});
    displayField_setPosition(menu->titlebar, (sfVector2f){centerX, titleY});

    displayField_setSize(menu->colorLabel, labelSize);
    displayField_setOrigin(menu->colorLabel, (sfVector2f){labelSize.x / 2.0f, labelSize.y / 2.0f});
    displayField_setPosition(menu->colorLabel, (sfVector2f){centerX, colorLabelY});

    button_setSize(menu->color, buttonSize);
    button_setOrigin(menu->color, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->color, (sfVector2f){centerX, colorY});

    displayField_setSize(menu->engineLevelLabel, labelSize);
    displayField_setOrigin(menu->engineLevelLabel, (sfVector2f){labelSize.x / 2.0f, labelSize.y / 2.0f});
    displayField_setPosition(menu->engineLevelLabel, (sfVector2f){centerX, engineLabelY});

    textField_setSize(menu->engineLevelField, fieldSize);
    textField_setOrigin(menu->engineLevelField, (sfVector2f){fieldSize.x / 2.0f, fieldSize.y / 2.0f});
    textField_setPosition(menu->engineLevelField, (sfVector2f){centerX, fieldY});

    displayField_setSize(menu->noticeLabel, noticeSize);
    displayField_setOrigin(menu->noticeLabel, (sfVector2f){noticeSize.x / 2.0f, noticeSize.y / 2.0f});
    displayField_setPosition(menu->noticeLabel, (sfVector2f){centerX, noticeY});

    button_setSize(menu->play, buttonSize);
    button_setOrigin(menu->play, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->play, (sfVector2f){centerX, playY});

    button_setSize(menu->back, buttonSize);
    button_setOrigin(menu->back, (sfVector2f){buttonSize.x / 2.0f, buttonSize.y / 2.0f});
    button_setPosition(menu->back, (sfVector2f){centerX, backY});

    displayField_setSize(menu->errorLabel, errorSize);
    displayField_setOrigin(menu->errorLabel, (sfVector2f){errorSize.x / 2.0f, errorSize.y / 2.0f});
    displayField_setPosition(menu->errorLabel, (sfVector2f){centerX, errorY});
}

static void enginePveSetupMenu_setActions(enginePveSetupMenu* menu)
{
    button_setAction(menu->color, enginePveSetupMenu_onColor, menu);
    button_setAction(menu->play, enginePveSetupMenu_onPlay, menu);
    button_setAction(menu->back, enginePveSetupMenu_onBack, menu);
}

static sfBool enginePveSetupMenu_parseUnsigned(const char* string, unsigned int* value)
{
    unsigned int i;
    unsigned int result;

    if (string == NULL || string[0] == '\0')
        return sfFalse;

    result = 0;
    i = 0;

    while (string[i] != '\0')
    {
        if (string[i] < '0' || string[i] > '9')
            return sfFalse;

        result = result * 10 + (unsigned int)(string[i] - '0');

        i++;
    }

    *value = result;

    return sfTrue;
}

static sfBool enginePveSetupMenu_validate(enginePveSetupMenu* menu)
{
    unsigned int engineLevel;

    if (!enginePveSetupMenu_parseUnsigned(textField_getTextString(menu->engineLevelField), &engineLevel))
    {
        displayField_setTextString(menu->errorLabel, "Only numbers accepted");
        return sfFalse;
    }

    if (engineLevel > 20)
    {
        displayField_setTextString(menu->errorLabel, "Engine level must be 0-20");
        return sfFalse;
    }

    menu->data.engineLevel = (int)engineLevel;

    displayField_setTextString(menu->errorLabel, "");

    return sfTrue;
}

enginePveSetupMenu* enginePveSetupMenu_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    enginePveSetupMenu* menu;

    if (resources == NULL)
        return NULL;

    menu = malloc(sizeof(enginePveSetupMenu));

    if (menu == NULL)
        return NULL;

    menu->titlebar = NULL;

    menu->colorLabel = NULL;
    menu->engineLevelLabel = NULL;
    menu->noticeLabel = NULL;
    menu->errorLabel = NULL;

    menu->color = NULL;
    menu->engineLevelField = NULL;

    menu->play = NULL;
    menu->back = NULL;

    menu->data = vsEngineSetup_getDefault();

    menu->active = sfTrue;
    menu->action = enginePveSetupMenuActionNone;

    menu->titlebar = displayField_create();

    menu->colorLabel = displayField_create();
    menu->engineLevelLabel = displayField_create();
    menu->noticeLabel = displayField_create();
    menu->errorLabel = displayField_create();

    menu->color = button_create();
    menu->engineLevelField = textField_create();

    menu->play = button_create();
    menu->back = button_create();

    if (
        menu->titlebar == NULL ||
        menu->colorLabel == NULL ||
        menu->engineLevelLabel == NULL ||
        menu->noticeLabel == NULL ||
        menu->errorLabel == NULL ||
        menu->color == NULL ||
        menu->engineLevelField == NULL ||
        menu->play == NULL ||
        menu->back == NULL
    )
    {
        enginePveSetupMenu_destroy(menu);
        return NULL;
    }

    enginePveSetupMenu_applyResources(menu, resources);
    enginePveSetupMenu_setTexts(menu);
    enginePveSetupMenu_setStyle(menu);
    enginePveSetupMenu_setLayout(menu, topLeft, bottomRight);
    enginePveSetupMenu_setActions(menu);

    return menu;
}

void enginePveSetupMenu_destroy(enginePveSetupMenu* menu)
{
    if (menu == NULL)
        return;

    if (menu->titlebar != NULL)
        displayField_destroy(menu->titlebar);

    if (menu->colorLabel != NULL)
        displayField_destroy(menu->colorLabel);

    if (menu->engineLevelLabel != NULL)
        displayField_destroy(menu->engineLevelLabel);

    if (menu->noticeLabel != NULL)
        displayField_destroy(menu->noticeLabel);

    if (menu->errorLabel != NULL)
        displayField_destroy(menu->errorLabel);

    if (menu->color != NULL)
        button_destroy(menu->color);

    if (menu->engineLevelField != NULL)
        textField_destroy(menu->engineLevelField);

    if (menu->play != NULL)
        button_destroy(menu->play);

    if (menu->back != NULL)
        button_destroy(menu->back);

    free(menu);
}

void enginePveSetupMenu_updateMouse(enginePveSetupMenu* menu, const Mouse* mouse)
{
    sfVector2i position;

    if (menu == NULL || mouse == NULL)
        return;

    if (!menu->active)
        return;

    textField_updateMouse(menu->engineLevelField, mouse);

    if (mouse_wasJustPressed(mouse))
    {
        position = mouse_getPosition(mouse);

        if (textField_containsPoint(menu->engineLevelField, position))
            textField_select(menu->engineLevelField);
        else
            textField_deselect(menu->engineLevelField);
    }

    button_updateMouse(menu->color, mouse);
    button_updateMouse(menu->play, mouse);
    button_updateMouse(menu->back, mouse);
}

void enginePveSetupMenu_updateKeyboard(enginePveSetupMenu* menu, const sfEvent* event)
{
    if (menu == NULL || event == NULL)
        return;

    if (!menu->active)
        return;

    textField_updateKeyboard(menu->engineLevelField, event);
}

void enginePveSetupMenu_setActive(enginePveSetupMenu* menu, sfBool active)
{
    if (menu == NULL)
        return;

    menu->active = active;
}

sfBool enginePveSetupMenu_isActive(const enginePveSetupMenu* menu)
{
    if (menu == NULL)
        return sfFalse;

    return menu->active;
}

enginePveSetupMenuAction enginePveSetupMenu_getAction(const enginePveSetupMenu* menu)
{
    if (menu == NULL)
        return enginePveSetupMenuActionNone;

    return menu->action;
}

enginePveSetupMenuAction enginePveSetupMenu_consumeAction(enginePveSetupMenu* menu)
{
    enginePveSetupMenuAction action;

    if (menu == NULL)
        return enginePveSetupMenuActionNone;

    action = menu->action;
    menu->action = enginePveSetupMenuActionNone;

    return action;
}

VsEngineSetup enginePveSetupMenu_getData(const enginePveSetupMenu* menu)
{
    if (menu == NULL)
        return vsEngineSetup_getDefault();

    return menu->data;
}

void enginePveSetupMenu_draw(sfRenderWindow* window, const enginePveSetupMenu* menu)
{
    if (window == NULL || menu == NULL)
        return;

    if (!menu->active)
        return;

    displayField_draw(window, menu->titlebar);

    displayField_draw(window, menu->colorLabel);
    button_draw(window, menu->color);

    displayField_draw(window, menu->engineLevelLabel);
    textField_draw(window, menu->engineLevelField);

    displayField_draw(window, menu->noticeLabel);

    button_draw(window, menu->play);
    button_draw(window, menu->back);

    displayField_draw(window, menu->errorLabel);
}