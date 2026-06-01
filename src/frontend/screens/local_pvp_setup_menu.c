#include "frontend/screens/local_pvp_setup_menu.h"

#include "frontend/ui/button.h"
#include "frontend/ui/display_field.h"
#include "frontend/ui/text_field.h"

#include <stdlib.h>

#define MENU_BUTTON_TEXTURE_WIDTH 894
#define MENU_BUTTON_TEXTURE_HEIGHT 234

#define MENU_TITLE_TEXTURE_WIDTH 1596
#define MENU_TITLE_TEXTURE_HEIGHT 316

struct localPvPSetupMenu
{
    DisplayField* titlebar;

    DisplayField* timeLabel;
    DisplayField* plusLabel;
    DisplayField* noticeLabel;
    DisplayField* errorLabel;

    TextField* startingMinutesField;
    TextField* incrementSecondsField;

    Button* play;
    Button* back;

    LocalPvPSetup data;

    sfBool active;

    LocalPvPSetupMenuAction action;
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

static void localPvPSetupMenu_onPlay(void*);
static void localPvPSetupMenu_onBack(void*);

static void localPvPSetupMenu_applyResources(localPvPSetupMenu*, const Resources*);
static void localPvPSetupMenu_setTexts(localPvPSetupMenu*);
static void localPvPSetupMenu_setStyle(localPvPSetupMenu*);
static void localPvPSetupMenu_setLayout(localPvPSetupMenu*, sfVector2i, sfVector2i);
static void localPvPSetupMenu_setActions(localPvPSetupMenu*);

static void localPvPSetupMenu_setupButtonTexture(Button*, const sfTexture*);
static void localPvPSetupMenu_setupButtonText(Button*, const sfFont*);
static void localPvPSetupMenu_setupTextField(TextField*, const sfFont*);
static void localPvPSetupMenu_setupPlainText(DisplayField*, const sfFont*, unsigned int, sfColor);

static sfBool localPvPSetupMenu_parseUnsigned(const char*, unsigned int*);
static sfBool localPvPSetupMenu_validate(localPvPSetupMenu*);

static void localPvPSetupMenu_onPlay(void* data)
{
    localPvPSetupMenu* menu;

    menu = data;

    if (localPvPSetupMenu_validate(menu))
        menu->action = localPvPSetupMenuActionStart;
}

static void localPvPSetupMenu_onBack(void* data)
{
    ((localPvPSetupMenu*)data)->action = localPvPSetupMenuActionBack;
}

static void localPvPSetupMenu_setupButtonTexture(Button* button, const sfTexture* texture)
{
    button_setTexture(button, texture, sfFalse);

    button_setTextureRect_onIdle(button, MENU_BUTTON_IDLE_RECT);
    button_setTextureRect_onHover(button, MENU_BUTTON_HOVER_RECT);
    button_setTextureRect_onPress(button, MENU_BUTTON_PRESS_RECT);
}

static void localPvPSetupMenu_setupButtonText(Button* button, const sfFont* font)
{
    button_setTextFont(button, font);
    button_setTextColor(button, sfWhite);
    button_setCharacterSize(button, 32);
    button_setLetterSpacing(button, 1.3f);
}

static void localPvPSetupMenu_setupTextField(TextField* textField, const sfFont* font)
{
    textField_setTextFont(textField, font);
    textField_setCharacterSize(textField, 32);
    textField_setLetterSpacing(textField, 1.0f);
    textField_setTextColor(textField, sfBlack);
    textField_setTextPadding(textField, (sfVector2f){12.0f, 0.0f});
    textField_setFillColor(textField, sfWhite);
    textField_setOutlineColor(textField, sfBlack);
    textField_setOutlineThickness(textField, 2.0f);
}

static void localPvPSetupMenu_setupPlainText(DisplayField* field, const sfFont* font, unsigned int size, sfColor color)
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

static void localPvPSetupMenu_applyResources(localPvPSetupMenu* menu, const Resources* resources)
{
    const sfTexture* buttonTexture;
    const sfTexture* titleBoardTexture;

    const sfFont* titleFont;
    const sfFont* buttonFont;
    const sfFont* plainFont;

    buttonTexture = resources_getTexture(resources, resourceTextureButtons);
    titleBoardTexture = resources_getTexture(resources, resourceTextureTitleBoard);

    titleFont = resources_getFont(resources, resourceFontCinzelSemiBold);
    buttonFont = resources_getFont(resources, resourceFontCinzelMedium);
    plainFont = resources_getFont(resources, resourceFontJetBrainsMonoRegular);

    displayField_setTexture(menu->titlebar, titleBoardTexture, sfFalse);
    displayField_setTextureRect(menu->titlebar, MENU_TITLE_RECT);
    displayField_setTextFont(menu->titlebar, titleFont);

    localPvPSetupMenu_setupPlainText(menu->timeLabel, titleFont, 36, sfWhite);
    localPvPSetupMenu_setupPlainText(menu->plusLabel, titleFont, 42, sfWhite);
    localPvPSetupMenu_setupPlainText(menu->noticeLabel, plainFont, 18, sfWhite);
    localPvPSetupMenu_setupPlainText(menu->errorLabel, plainFont, 20, sfRed);

    localPvPSetupMenu_setupTextField(menu->startingMinutesField, plainFont);
    localPvPSetupMenu_setupTextField(menu->incrementSecondsField, plainFont);

    localPvPSetupMenu_setupButtonTexture(menu->play, buttonTexture);
    localPvPSetupMenu_setupButtonTexture(menu->back, buttonTexture);

    localPvPSetupMenu_setupButtonText(menu->play, buttonFont);
    localPvPSetupMenu_setupButtonText(menu->back, buttonFont);
}

static void localPvPSetupMenu_setTexts(localPvPSetupMenu* menu)
{
    displayField_setTextString(menu->titlebar, "LOCAL PVP");
    displayField_setTextString(menu->timeLabel, "TIME");
    displayField_setTextString(menu->plusLabel, "+");
    displayField_setTextString(menu->noticeLabel, "Only numbers accepted. First field = minutes, second field = seconds per move.");
    displayField_setTextString(menu->errorLabel, "");

    textField_setTextString(menu->startingMinutesField, "15");
    textField_setTextString(menu->incrementSecondsField, "0");

    button_setTextString(menu->play, "Play");
    button_setTextString(menu->back, "Back");
}

static void localPvPSetupMenu_setStyle(localPvPSetupMenu* menu)
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

    button_setFillColor(menu->play, sfWhite);
    button_setFillColor(menu->back, sfWhite);

    button_setOutlineThickness(menu->play, 0.0f);
    button_setOutlineThickness(menu->back, 0.0f);
}

static void localPvPSetupMenu_setLayout(localPvPSetupMenu* menu, sfVector2i topLeft, sfVector2i bottomRight)
{
    float areaWidth;
    float areaHeight;
    float centerX;

    sfVector2f titleSize;
    sfVector2f labelSize;
    sfVector2f noticeSize;
    sfVector2f fieldSize;
    sfVector2f plusSize;
    sfVector2f buttonSize;
    sfVector2f errorSize;

    float titleY;
    float labelY;
    float fieldY;
    float noticeY;
    float playY;
    float backY;
    float errorY;

    float fieldGap;
    float leftFieldX;
    float plusX;
    float rightFieldX;

    areaWidth = (float)(bottomRight.x - topLeft.x);
    areaHeight = (float)(bottomRight.y - topLeft.y);

    centerX = (float)topLeft.x + areaWidth / 2.0f;

    titleSize = (sfVector2f){areaWidth * 0.58f, areaHeight * 0.15f};
    labelSize = (sfVector2f){areaWidth * 0.30f, areaHeight * 0.07f};
    noticeSize = (sfVector2f){areaWidth * 0.70f, areaHeight * 0.05f};
    fieldSize = (sfVector2f){areaWidth * 0.14f, areaHeight * 0.075f};
    plusSize = (sfVector2f){areaWidth * 0.05f, areaHeight * 0.075f};
    buttonSize = (sfVector2f){areaWidth * 0.28f, areaHeight * 0.085f};
    errorSize = (sfVector2f){areaWidth * 0.50f, areaHeight * 0.05f};

    titleY = (float)topLeft.y + areaHeight * 0.16f;
    labelY = (float)topLeft.y + areaHeight * 0.34f;
    fieldY = (float)topLeft.y + areaHeight * 0.43f;
    noticeY = (float)topLeft.y + areaHeight * 0.52f;
    playY = (float)topLeft.y + areaHeight * 0.65f;
    backY = (float)topLeft.y + areaHeight * 0.76f;
    errorY = (float)topLeft.y + areaHeight * 0.87f;

    fieldGap = areaWidth * 0.035f;
    plusX = centerX;
    leftFieldX = centerX - fieldSize.x / 2.0f - plusSize.x / 2.0f - fieldGap;
    rightFieldX = centerX + fieldSize.x / 2.0f + plusSize.x / 2.0f + fieldGap;

    displayField_setSize(menu->titlebar, titleSize);
    displayField_setOrigin(menu->titlebar, (sfVector2f){titleSize.x / 2.0f, titleSize.y / 2.0f});
    displayField_setPosition(menu->titlebar, (sfVector2f){centerX, titleY});

    displayField_setSize(menu->timeLabel, labelSize);
    displayField_setOrigin(menu->timeLabel, (sfVector2f){labelSize.x / 2.0f, labelSize.y / 2.0f});
    displayField_setPosition(menu->timeLabel, (sfVector2f){centerX, labelY});

    textField_setSize(menu->startingMinutesField, fieldSize);
    textField_setOrigin(menu->startingMinutesField, (sfVector2f){fieldSize.x / 2.0f, fieldSize.y / 2.0f});
    textField_setPosition(menu->startingMinutesField, (sfVector2f){leftFieldX, fieldY});

    displayField_setSize(menu->plusLabel, plusSize);
    displayField_setOrigin(menu->plusLabel, (sfVector2f){plusSize.x / 2.0f, plusSize.y / 2.0f});
    displayField_setPosition(menu->plusLabel, (sfVector2f){plusX, fieldY});

    textField_setSize(menu->incrementSecondsField, fieldSize);
    textField_setOrigin(menu->incrementSecondsField, (sfVector2f){fieldSize.x / 2.0f, fieldSize.y / 2.0f});
    textField_setPosition(menu->incrementSecondsField, (sfVector2f){rightFieldX, fieldY});

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

static void localPvPSetupMenu_setActions(localPvPSetupMenu* menu)
{
    button_setAction(menu->play, localPvPSetupMenu_onPlay, menu);
    button_setAction(menu->back, localPvPSetupMenu_onBack, menu);
}

static sfBool localPvPSetupMenu_parseUnsigned(const char* string, unsigned int* value)
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

static sfBool localPvPSetupMenu_validate(localPvPSetupMenu* menu)
{
    unsigned int startingMinutes;
    unsigned int incrementSeconds;

    if (
        !localPvPSetupMenu_parseUnsigned(textField_getTextString(menu->startingMinutesField), &startingMinutes) ||
        !localPvPSetupMenu_parseUnsigned(textField_getTextString(menu->incrementSecondsField), &incrementSeconds)
    )
    {
        displayField_setTextString(menu->errorLabel, "Only numbers accepted");
        return sfFalse;
    }

    if (startingMinutes == 0)
    {
        displayField_setTextString(menu->errorLabel, "Starting time must be greater than 0");
        return sfFalse;
    }

    menu->data.startingTimeSeconds = startingMinutes * 60;
    menu->data.incrementSeconds = incrementSeconds;

    displayField_setTextString(menu->errorLabel, "");

    return sfTrue;
}

localPvPSetupMenu* localPvPSetupMenu_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    localPvPSetupMenu* menu;

    if (resources == NULL)
        return NULL;

    menu = malloc(sizeof(localPvPSetupMenu));

    if (menu == NULL)
        return NULL;

    menu->titlebar = NULL;

    menu->timeLabel = NULL;
    menu->plusLabel = NULL;
    menu->noticeLabel = NULL;
    menu->errorLabel = NULL;

    menu->startingMinutesField = NULL;
    menu->incrementSecondsField = NULL;

    menu->play = NULL;
    menu->back = NULL;

    menu->data = localPvPSetup_getDefault();

    menu->active = sfTrue;
    menu->action = localPvPSetupMenuActionNone;

    menu->titlebar = displayField_create();

    menu->timeLabel = displayField_create();
    menu->plusLabel = displayField_create();
    menu->noticeLabel = displayField_create();
    menu->errorLabel = displayField_create();

    menu->startingMinutesField = textField_create();
    menu->incrementSecondsField = textField_create();

    menu->play = button_create();
    menu->back = button_create();

    if (
        menu->titlebar == NULL ||
        menu->timeLabel == NULL ||
        menu->plusLabel == NULL ||
        menu->noticeLabel == NULL ||
        menu->errorLabel == NULL ||
        menu->startingMinutesField == NULL ||
        menu->incrementSecondsField == NULL ||
        menu->play == NULL ||
        menu->back == NULL
    )
    {
        localPvPSetupMenu_destroy(menu);
        return NULL;
    }

    localPvPSetupMenu_applyResources(menu, resources);
    localPvPSetupMenu_setTexts(menu);
    localPvPSetupMenu_setStyle(menu);
    localPvPSetupMenu_setLayout(menu, topLeft, bottomRight);
    localPvPSetupMenu_setActions(menu);

    return menu;
}

void localPvPSetupMenu_destroy(localPvPSetupMenu* menu)
{
    if (menu == NULL)
        return;

    if (menu->titlebar != NULL)
        displayField_destroy(menu->titlebar);

    if (menu->timeLabel != NULL)
        displayField_destroy(menu->timeLabel);

    if (menu->plusLabel != NULL)
        displayField_destroy(menu->plusLabel);

    if (menu->noticeLabel != NULL)
        displayField_destroy(menu->noticeLabel);

    if (menu->errorLabel != NULL)
        displayField_destroy(menu->errorLabel);

    if (menu->startingMinutesField != NULL)
        textField_destroy(menu->startingMinutesField);

    if (menu->incrementSecondsField != NULL)
        textField_destroy(menu->incrementSecondsField);

    if (menu->play != NULL)
        button_destroy(menu->play);

    if (menu->back != NULL)
        button_destroy(menu->back);

    free(menu);
}

void localPvPSetupMenu_updateMouse(localPvPSetupMenu* menu, const Mouse* mouse)
{
    sfVector2i position;

    if (menu == NULL || mouse == NULL)
        return;

    if (!menu->active)
        return;

    textField_updateMouse(menu->startingMinutesField, mouse);
    textField_updateMouse(menu->incrementSecondsField, mouse);

    if (mouse_wasJustPressed(mouse))
    {
        position = mouse_getPosition(mouse);

        if (textField_containsPoint(menu->startingMinutesField, position))
        {
            textField_select(menu->startingMinutesField);
            textField_deselect(menu->incrementSecondsField);
        }
        else if (textField_containsPoint(menu->incrementSecondsField, position))
        {
            textField_deselect(menu->startingMinutesField);
            textField_select(menu->incrementSecondsField);
        }
        else
        {
            textField_deselect(menu->startingMinutesField);
            textField_deselect(menu->incrementSecondsField);
        }
    }

    button_updateMouse(menu->play, mouse);
    button_updateMouse(menu->back, mouse);
}

void localPvPSetupMenu_updateKeyboard(localPvPSetupMenu* menu, const sfEvent* event)
{
    if (menu == NULL || event == NULL)
        return;

    if (!menu->active)
        return;

    textField_updateKeyboard(menu->startingMinutesField, event);
    textField_updateKeyboard(menu->incrementSecondsField, event);
}

void localPvPSetupMenu_setActive(localPvPSetupMenu* menu, sfBool active)
{
    if (menu == NULL)
        return;

    menu->active = active;
}

sfBool localPvPSetupMenu_isActive(const localPvPSetupMenu* menu)
{
    if (menu == NULL)
        return sfFalse;

    return menu->active;
}

LocalPvPSetupMenuAction localPvPSetupMenu_getAction(const localPvPSetupMenu* menu)
{
    if (menu == NULL)
        return localPvPSetupMenuActionNone;

    return menu->action;
}

LocalPvPSetupMenuAction localPvPSetupMenu_consumeAction(localPvPSetupMenu* menu)
{
    LocalPvPSetupMenuAction action;

    if (menu == NULL)
        return localPvPSetupMenuActionNone;

    action = menu->action;
    menu->action = localPvPSetupMenuActionNone;

    return action;
}

LocalPvPSetup localPvPSetupMenu_getData(const localPvPSetupMenu* menu)
{
    if (menu == NULL)
        return localPvPSetup_getDefault();

    return menu->data;
}

void localPvPSetupMenu_draw(sfRenderWindow* window, const localPvPSetupMenu* menu)
{
    if (window == NULL || menu == NULL)
        return;

    if (!menu->active)
        return;

    displayField_draw(window, menu->titlebar);

    displayField_draw(window, menu->timeLabel);

    textField_draw(window, menu->startingMinutesField);
    displayField_draw(window, menu->plusLabel);
    textField_draw(window, menu->incrementSecondsField);

    displayField_draw(window, menu->noticeLabel);

    button_draw(window, menu->play);
    button_draw(window, menu->back);

    displayField_draw(window, menu->errorLabel);
}