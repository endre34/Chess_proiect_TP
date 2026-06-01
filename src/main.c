#include <stdio.h>

#include <SFML/Graphics.h>

#include "frontend/screen_manager.h"
#include "input/mouse.h"
#include "resources/resources.h"

#define WINDOW_WIDTH  1600
#define WINDOW_HEIGHT 900

#define TEST_TOP_LEFT_X 0
#define TEST_TOP_LEFT_Y 0
#define TEST_BOTTOM_RIGHT_X WINDOW_WIDTH
#define TEST_BOTTOM_RIGHT_Y WINDOW_HEIGHT

#define EXPECTED_DEFAULT_STARTING_TIME_SECONDS (15 * 60)
#define EXPECTED_DEFAULT_INCREMENT_SECONDS 0
#define EXPECTED_DEFAULT_ENGINE_LEVEL 5

typedef struct TestResults
{
    unsigned int passed;
    unsigned int failed;

} TestResults;

typedef enum MainMenuPoint
{
    mainMenuPointPlay,
    mainMenuPointSettings,
    mainMenuPointCredits,
    mainMenuPointExit

} MainMenuPoint;

typedef enum SettingsMenuPoint
{
    settingsMenuPointSound,
    settingsMenuPointFullscreen,
    settingsMenuPointControls,
    settingsMenuPointBack

} SettingsMenuPoint;

typedef enum GameSetupMenuPoint
{
    gameSetupMenuPointLocalPvP,
    gameSetupMenuPointVsEngine,
    gameSetupMenuPointBack

} GameSetupMenuPoint;

typedef enum LocalPvPSetupMenuPoint
{
    localPvPSetupPointStartingMinutesField,
    localPvPSetupPointIncrementSecondsField,
    localPvPSetupPointPlay,
    localPvPSetupPointBack

} LocalPvPSetupMenuPoint;

typedef enum EnginePvESetupMenuPoint
{
    enginePvESetupPointColor,
    enginePvESetupPointEngineLevelField,
    enginePvESetupPointPlay,
    enginePvESetupPointBack

} EnginePvESetupMenuPoint;

static void print_section(const char* title)
{
    printf("\n============================================================\n");
    printf("%s\n", title);
    printf("============================================================\n");
}

static void test_check(TestResults* results, sfBool condition, const char* message)
{
    if (condition)
    {
        ++results->passed;
        printf("[PASS] %s\n", message);
        return;
    }

    ++results->failed;
    printf("[FAIL] %s\n", message);
}

static const char* bool_toString(sfBool value)
{
    return value ? "true" : "false";
}

static const char* screenId_toString(ScreenId screen)
{
    switch (screen)
    {
        case screenIdMainMenu:
            return "Main menu";

        case screenIdSettingsMenu:
            return "Settings menu";

        case screenIdControlsMenu:
            return "Controls menu";

        case screenIdCreditsMenu:
            return "Credits menu";

        case screenIdGameSetupMenu:
            return "Game setup menu";

        case screenIdLocalPvPSetupMenu:
            return "Local PvP setup menu";

        case screenIdEnginePvESetupMenu:
            return "Engine PvE setup menu";

        default:
            return "Unknown screen";
    }
}

static const char* requestType_toString(ScreenManagerRequestType type)
{
    switch (type)
    {
        case screenManagerRequestNone:
            return "None";

        case screenManagerRequestExit:
            return "Exit";

        case screenManagerRequestStartGame:
            return "Start game";

        case screenManagerRequestApplySettings:
            return "Apply settings";

        default:
            return "Unknown request";
    }
}

static const char* gameMode_toString(GameMode mode)
{
    switch (mode)
    {
        case gameModeLocalPvP:
            return "Local PvP";

        case gameModeVsEngine:
            return "Vs Engine";

        default:
            return "Unknown game mode";
    }
}

static const char* playerColor_toString(PlayerColor color)
{
    switch (color)
    {
        case playerColorWhite:
            return "White";

        case playerColorBlack:
            return "Black";

        case playerColorRandom:
            return "Random";

        default:
            return "Unknown player color";
    }
}

static sfVector2i make_point(float x, float y)
{
    return (sfVector2i){(int)(x + 0.5f), (int)(y + 0.5f)};
}

static float area_width(void)
{
    return (float)(TEST_BOTTOM_RIGHT_X - TEST_TOP_LEFT_X);
}

static float area_height(void)
{
    return (float)(TEST_BOTTOM_RIGHT_Y - TEST_TOP_LEFT_Y);
}

static float center_x(void)
{
    return (float)TEST_TOP_LEFT_X + area_width() / 2.0f;
}

static sfVector2i mainMenu_point(MainMenuPoint point)
{
    float firstButtonY;
    float buttonGap;

    firstButtonY = (float)TEST_TOP_LEFT_Y + area_height() * 0.40f;
    buttonGap = area_height() * 0.115f;

    switch (point)
    {
        case mainMenuPointPlay:
            return make_point(center_x(), firstButtonY);

        case mainMenuPointSettings:
            return make_point(center_x(), firstButtonY + buttonGap);

        case mainMenuPointCredits:
            return make_point(center_x(), firstButtonY + 2.0f * buttonGap);

        case mainMenuPointExit:
            return make_point(center_x(), firstButtonY + 3.0f * buttonGap);

        default:
            return make_point(0.0f, 0.0f);
    }
}

static sfVector2i settingsMenu_point(SettingsMenuPoint point)
{
    float firstButtonY;
    float buttonGap;

    firstButtonY = (float)TEST_TOP_LEFT_Y + area_height() * 0.40f;
    buttonGap = area_height() * 0.11f;

    switch (point)
    {
        case settingsMenuPointSound:
            return make_point(center_x(), firstButtonY);

        case settingsMenuPointFullscreen:
            return make_point(center_x(), firstButtonY + buttonGap);

        case settingsMenuPointControls:
            return make_point(center_x(), firstButtonY + 2.0f * buttonGap);

        case settingsMenuPointBack:
            return make_point(center_x(), firstButtonY + 3.0f * buttonGap);

        default:
            return make_point(0.0f, 0.0f);
    }
}

static sfVector2i simpleBackMenu_point(void)
{
    return make_point(center_x(), (float)TEST_TOP_LEFT_Y + area_height() * 0.50f);
}

static sfVector2i gameSetupMenu_point(GameSetupMenuPoint point)
{
    float firstButtonY;
    float buttonGap;

    firstButtonY = (float)TEST_TOP_LEFT_Y + area_height() * 0.43f;
    buttonGap = area_height() * 0.11f;

    switch (point)
    {
        case gameSetupMenuPointLocalPvP:
            return make_point(center_x(), firstButtonY);

        case gameSetupMenuPointVsEngine:
            return make_point(center_x(), firstButtonY + buttonGap);

        case gameSetupMenuPointBack:
            return make_point(center_x(), firstButtonY + 2.0f * buttonGap);

        default:
            return make_point(0.0f, 0.0f);
    }
}

static sfVector2i localPvPSetupMenu_point(LocalPvPSetupMenuPoint point)
{
    float fieldSizeX;
    float plusSizeX;
    float fieldGap;
    float leftFieldX;
    float rightFieldX;

    fieldSizeX = area_width() * 0.14f;
    plusSizeX = area_width() * 0.05f;
    fieldGap = area_width() * 0.035f;

    leftFieldX = center_x() - fieldSizeX / 2.0f - plusSizeX / 2.0f - fieldGap;
    rightFieldX = center_x() + fieldSizeX / 2.0f + plusSizeX / 2.0f + fieldGap;

    switch (point)
    {
        case localPvPSetupPointStartingMinutesField:
            return make_point(leftFieldX, (float)TEST_TOP_LEFT_Y + area_height() * 0.43f);

        case localPvPSetupPointIncrementSecondsField:
            return make_point(rightFieldX, (float)TEST_TOP_LEFT_Y + area_height() * 0.43f);

        case localPvPSetupPointPlay:
            return make_point(center_x(), (float)TEST_TOP_LEFT_Y + area_height() * 0.65f);

        case localPvPSetupPointBack:
            return make_point(center_x(), (float)TEST_TOP_LEFT_Y + area_height() * 0.76f);

        default:
            return make_point(0.0f, 0.0f);
    }
}

static sfVector2i enginePvESetupMenu_point(EnginePvESetupMenuPoint point)
{
    switch (point)
    {
        case enginePvESetupPointColor:
            return make_point(center_x(), (float)TEST_TOP_LEFT_Y + area_height() * 0.39f);

        case enginePvESetupPointEngineLevelField:
            return make_point(center_x(), (float)TEST_TOP_LEFT_Y + area_height() * 0.58f);

        case enginePvESetupPointPlay:
            return make_point(center_x(), (float)TEST_TOP_LEFT_Y + area_height() * 0.76f);

        case enginePvESetupPointBack:
            return make_point(center_x(), (float)TEST_TOP_LEFT_Y + area_height() * 0.86f);

        default:
            return make_point(0.0f, 0.0f);
    }
}

static Mouse make_testMouse(sfVector2i position, sfBool pressed, sfBool justPressed, sfBool justReleased)
{
    Mouse mouse;

    mouse.button = sfMouseLeft;
    mouse.pressed = pressed;
    mouse.justPressed = justPressed;
    mouse.justReleased = justReleased;
    mouse.position = position;

    return mouse;
}

static void simulate_click(screenManager* manager, sfVector2i point, const char* label)
{
    Mouse pressMouse;
    Mouse releaseMouse;

    printf("[CLICK] %-42s at (%d, %d)\n", label, point.x, point.y);

    pressMouse = make_testMouse(point, sfTrue, sfTrue, sfFalse);
    screenManager_updateMouse(manager, &pressMouse);

    releaseMouse = make_testMouse(point, sfFalse, sfFalse, sfTrue);
    screenManager_updateMouse(manager, &releaseMouse);
}

static void send_key_press(screenManager* manager, sfKeyCode key)
{
    sfEvent event;

    event = (sfEvent){0};
    event.type = sfEvtKeyPressed;
    event.key.code = key;

    screenManager_updateKeyboard(manager, &event);
}

static void send_text_char(screenManager* manager, char character)
{
    sfEvent event;

    event = (sfEvent){0};
    event.type = sfEvtTextEntered;
    event.text.unicode = (sfUint32)character;

    screenManager_updateKeyboard(manager, &event);
}

static void send_text_string(screenManager* manager, const char* string)
{
    unsigned int i;

    i = 0;

    while (string[i] != '\0')
    {
        send_text_char(manager, string[i]);
        i++;
    }
}

static void send_backspaces(screenManager* manager, unsigned int count)
{
    unsigned int i;

    for (i = 0; i < count; i++)
        send_key_press(manager, sfKeyBack);
}

static void expect_screen(TestResults* results, const screenManager* manager, ScreenId expected, const char* message)
{
    ScreenId actual;

    actual = screenManager_getScreen(manager);

    printf(
        "[INFO] Screen: expected=%s, actual=%s\n",
        screenId_toString(expected),
        screenId_toString(actual)
    );

    test_check(results, actual == expected, message);
}

static void expect_requestType(
    TestResults* results,
    ScreenManagerRequest request,
    ScreenManagerRequestType expected,
    const char* message
)
{
    printf(
        "[INFO] Request: expected=%s, actual=%s\n",
        requestType_toString(expected),
        requestType_toString(request.type)
    );

    test_check(results, request.type == expected, message);
}

static void discard_request(screenManager* manager)
{
    (void)screenManager_consumeRequest(manager);
}

static void print_requestDetails(ScreenManagerRequest request)
{
    printf("[REQUEST] %s\n", requestType_toString(request.type));

    if (request.type == screenManagerRequestApplySettings)
    {
        printf(
            "          sound=%s, fullscreen=%s\n",
            bool_toString(request.data.settings.soundEnabled),
            bool_toString(request.data.settings.fullscreenEnabled)
        );
    }

    if (request.type == screenManagerRequestStartGame)
    {
        printf("          mode=%s\n", gameMode_toString(request.data.gameSetup.mode));

        if (request.data.gameSetup.mode == gameModeLocalPvP)
        {
            printf(
                "          startingTimeSeconds=%u, incrementSeconds=%u\n",
                request.data.gameSetup.data.localPvP.startingTimeSeconds,
                request.data.gameSetup.data.localPvP.incrementSeconds
            );
        }

        if (request.data.gameSetup.mode == gameModeVsEngine)
        {
            printf(
                "          playerColor=%s, engineLevel=%d\n",
                playerColor_toString(request.data.gameSetup.data.vsEngine.playerColor),
                request.data.gameSetup.data.vsEngine.engineLevel
            );
        }
    }
}

static void test_resourceTexture(
    TestResults* results,
    const Resources* resources,
    ResourceTextureId id,
    const char* name
)
{
    const sfTexture* texture;
    sfVector2u size;

    texture = resources_getTexture(resources, id);

    test_check(results, texture != NULL, name);

    if (texture == NULL)
        return;

    size = sfTexture_getSize(texture);

    printf("[INFO] %-28s size=%ux%u smooth=%s\n", name, size.x, size.y, bool_toString(sfTexture_isSmooth(texture)));

    test_check(results, size.x > 0 && size.y > 0, "texture has non-zero dimensions");
    test_check(results, sfTexture_isSmooth(texture) == sfTrue, "texture smoothing is enabled");
}

static void test_resourceIcon(
    TestResults* results,
    const Resources* resources,
    ResourceIconId id,
    const char* name
)
{
    const sfTexture* icon;
    sfVector2u size;

    icon = resources_getIcon(resources, id);

    test_check(results, icon != NULL, name);

    if (icon == NULL)
        return;

    size = sfTexture_getSize(icon);

    printf("[INFO] %-28s size=%ux%u smooth=%s\n", name, size.x, size.y, bool_toString(sfTexture_isSmooth(icon)));

    test_check(results, size.x > 0 && size.y > 0, "icon has non-zero dimensions");
    test_check(results, sfTexture_isSmooth(icon) == sfTrue, "icon smoothing is enabled");
}

static void test_resourceFont(
    TestResults* results,
    const Resources* resources,
    ResourceFontId id,
    const char* name
)
{
    test_check(results, resources_getFont(resources, id) != NULL, name);
}

static void test_resources(TestResults* results, const Resources* resources)
{
    const sfTexture* firstTextureCall;
    const sfTexture* secondTextureCall;

    const sfFont* firstFontCall;
    const sfFont* secondFontCall;

    print_section("Resources");

    test_check(results, resources != NULL, "resources object exists");

    if (resources == NULL)
        return;

    test_resourceTexture(results, resources, resourceTextureBackground, "background texture loaded");
    test_resourceTexture(results, resources, resourceTextureBoard, "board texture loaded");
    test_resourceTexture(results, resources, resourceTextureButtons, "buttons texture loaded");
    test_resourceTexture(results, resources, resourceTexturePieces, "pieces texture loaded");
    test_resourceTexture(results, resources, resourceTextureSideboard, "sideboard texture loaded");
    test_resourceTexture(results, resources, resourceTextureTimeBoard, "time board texture loaded");
    test_resourceTexture(results, resources, resourceTextureTitleBoard, "title board texture loaded");

    test_resourceIcon(results, resources, resourceIconDraw, "draw icon loaded");
    test_resourceIcon(results, resources, resourceIconGameLogo, "game logo icon loaded");
    test_resourceIcon(results, resources, resourceIconResign, "resign icon loaded");
    test_resourceIcon(results, resources, resourceIconSettingWhite, "setting white icon loaded");
    test_resourceIcon(results, resources, resourceIconSettings, "settings icon loaded");
    test_resourceIcon(results, resources, resourceIconSfmlLogo, "SFML logo icon loaded");
    test_resourceIcon(results, resources, resourceIconStockfish, "Stockfish icon loaded");

    test_resourceFont(results, resources, resourceFontCinzelMedium, "Cinzel Medium font loaded");
    test_resourceFont(results, resources, resourceFontCinzelRegular, "Cinzel Regular font loaded");
    test_resourceFont(results, resources, resourceFontCinzelSemiBold, "Cinzel SemiBold font loaded");
    test_resourceFont(results, resources, resourceFontJetBrainsMonoItalic, "JetBrains Mono Italic font loaded");
    test_resourceFont(results, resources, resourceFontJetBrainsMonoLight, "JetBrains Mono Light font loaded");
    test_resourceFont(results, resources, resourceFontJetBrainsMonoLightItalic, "JetBrains Mono Light Italic font loaded");
    test_resourceFont(results, resources, resourceFontJetBrainsMonoRegular, "JetBrains Mono Regular font loaded");
    test_resourceFont(results, resources, resourceFontJetBrainsMonoSemiBold, "JetBrains Mono SemiBold font loaded");
    test_resourceFont(results, resources, resourceFontJetBrainsMonoSemiBoldItalic, "JetBrains Mono SemiBold Italic font loaded");

    test_check(results, resources_getTexture(resources, (ResourceTextureId)-1) == NULL, "invalid negative texture id returns NULL");
    test_check(results, resources_getTexture(resources, resourceTextureCount) == NULL, "invalid high texture id returns NULL");

    test_check(results, resources_getIcon(resources, (ResourceIconId)-1) == NULL, "invalid negative icon id returns NULL");
    test_check(results, resources_getIcon(resources, resourceIconCount) == NULL, "invalid high icon id returns NULL");

    test_check(results, resources_getFont(resources, (ResourceFontId)-1) == NULL, "invalid negative font id returns NULL");
    test_check(results, resources_getFont(resources, resourceFontCount) == NULL, "invalid high font id returns NULL");

    firstTextureCall = resources_getTexture(resources, resourceTextureButtons);
    secondTextureCall = resources_getTexture(resources, resourceTextureButtons);

    firstFontCall = resources_getFont(resources, resourceFontCinzelSemiBold);
    secondFontCall = resources_getFont(resources, resourceFontCinzelSemiBold);

    test_check(results, firstTextureCall != NULL && firstTextureCall == secondTextureCall, "texture getter returns stable borrowed pointer");
    test_check(results, firstFontCall != NULL && firstFontCall == secondFontCall, "font getter returns stable borrowed pointer");
}

static void test_mouse(TestResults* results)
{
    Mouse* mouse;
    Mouse* copy;

    print_section("Mouse object");

    mouse = mouse_create(sfMouseLeft);

    test_check(results, mouse != NULL, "mouse object created");

    if (mouse == NULL)
        return;

    test_check(results, mouse_getButton(mouse) == sfMouseLeft, "mouse button stored correctly");
    test_check(results, mouse_isPressed(mouse) == sfFalse, "mouse starts not pressed");
    test_check(results, mouse_wasJustPressed(mouse) == sfFalse, "mouse starts not just pressed");
    test_check(results, mouse_wasJustReleased(mouse) == sfFalse, "mouse starts not just released");

    test_check(
        results,
        mouse_getPosition(mouse).x == 0 && mouse_getPosition(mouse).y == 0,
        "mouse starts at position 0,0"
    );

    copy = mouse_copy(mouse);

    test_check(results, copy != NULL, "mouse copy created");

    if (copy != NULL)
    {
        test_check(results, mouse_getButton(copy) == mouse_getButton(mouse), "mouse copy preserves button");
        test_check(results, mouse_isPressed(copy) == mouse_isPressed(mouse), "mouse copy preserves pressed state");
        test_check(results, mouse_getPosition(copy).x == mouse_getPosition(mouse).x, "mouse copy preserves x position");
        test_check(results, mouse_getPosition(copy).y == mouse_getPosition(mouse).y, "mouse copy preserves y position");

        mouse_destroy(copy);
    }

    mouse_destroy(mouse);
}

static void test_screenDataDefaults(TestResults* results)
{
    SettingsData settings;
    LocalPvPSetup localPvP;
    VsEngineSetup vsEngine;
    GameSetupData gameSetup;

    print_section("Screen data defaults");

    settings = settingsData_getDefault();
    localPvP = localPvPSetup_getDefault();
    vsEngine = vsEngineSetup_getDefault();
    gameSetup = gameSetupData_getDefault();

    test_check(results, settings.soundEnabled == sfTrue, "default settings: sound enabled");
    test_check(results, settings.fullscreenEnabled == sfFalse, "default settings: fullscreen disabled");

    test_check(results, localPvP.startingTimeSeconds == EXPECTED_DEFAULT_STARTING_TIME_SECONDS, "default Local PvP starting time is 15 minutes");
    test_check(results, localPvP.incrementSeconds == EXPECTED_DEFAULT_INCREMENT_SECONDS, "default Local PvP increment is 0 seconds");

    test_check(results, vsEngine.playerColor == playerColorWhite, "default engine PvE color is white");
    test_check(results, vsEngine.engineLevel == EXPECTED_DEFAULT_ENGINE_LEVEL, "default engine level is 5");

    test_check(results, gameSetup.mode == gameModeLocalPvP, "default game setup mode is Local PvP");
    test_check(results, gameSetup.data.localPvP.startingTimeSeconds == EXPECTED_DEFAULT_STARTING_TIME_SECONDS, "default game setup Local PvP starting time is 15 minutes");
    test_check(results, gameSetup.data.localPvP.incrementSeconds == EXPECTED_DEFAULT_INCREMENT_SECONDS, "default game setup Local PvP increment is 0 seconds");
}

static void test_requestHelpers(TestResults* results)
{
    ScreenManagerRequest request;

    print_section("Screen manager request helpers");

    request = screenManagerRequest_getNone();

    test_check(results, request.type == screenManagerRequestNone, "screenManagerRequest_getNone returns None");
}

static void test_screenManagerNullSafety(TestResults* results)
{
    ScreenManagerRequest request;

    print_section("Screen manager NULL safety");

    test_check(
        results,
        screenManager_create(
            (sfVector2i){TEST_TOP_LEFT_X, TEST_TOP_LEFT_Y},
            (sfVector2i){TEST_BOTTOM_RIGHT_X, TEST_BOTTOM_RIGHT_Y},
            NULL
        ) == NULL,
        "screenManager_create rejects NULL resources"
    );

    screenManager_destroy(NULL);
    screenManager_updateMouse(NULL, NULL);
    screenManager_updateKeyboard(NULL, NULL);
    screenManager_setActive(NULL, sfTrue);
    screenManager_setScreen(NULL, screenIdCreditsMenu);
    screenManager_draw(NULL, NULL);

    test_check(results, screenManager_isActive(NULL) == sfFalse, "screenManager_isActive(NULL) returns false");
    test_check(results, screenManager_getScreen(NULL) == screenIdMainMenu, "screenManager_getScreen(NULL) returns main menu fallback");

    request = screenManager_getRequest(NULL);
    test_check(results, request.type == screenManagerRequestNone, "screenManager_getRequest(NULL) returns none");

    request = screenManager_consumeRequest(NULL);
    test_check(results, request.type == screenManagerRequestNone, "screenManager_consumeRequest(NULL) returns none");

    printf("[INFO] NULL safety calls completed without crashing.\n");
}

static void test_screenManagerCreation(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Screen manager creation and initial state");

    test_check(results, manager != NULL, "screen manager object created");

    if (manager == NULL)
        return;

    test_check(results, screenManager_isActive(manager) == sfTrue, "screen manager starts active");
    expect_screen(results, manager, screenIdMainMenu, "screen manager starts on main menu");

    request = screenManager_getRequest(manager);
    expect_requestType(results, request, screenManagerRequestNone, "screen manager starts with no pending request");

    request = screenManager_consumeRequest(manager);
    expect_requestType(results, request, screenManagerRequestNone, "consuming initial request returns none");

    request = screenManager_getRequest(manager);
    expect_requestType(results, request, screenManagerRequestNone, "request remains none after consuming initial request");

    screenManager_updateMouse(manager, NULL);
    screenManager_updateKeyboard(manager, NULL);
    screenManager_draw(NULL, manager);

    printf("[INFO] Safe calls with NULL input/window completed without crashing.\n");
}

static void test_screenManagerActiveState(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Screen manager active state");

    screenManager_setScreen(manager, screenIdMainMenu);
    discard_request(manager);

    screenManager_setActive(manager, sfFalse);

    test_check(results, screenManager_isActive(manager) == sfFalse, "screen manager can be deactivated");

    simulate_click(manager, mainMenu_point(mainMenuPointPlay), "inactive manager: click Main/Play");

    expect_screen(results, manager, screenIdMainMenu, "inactive manager ignores mouse updates");

    request = screenManager_getRequest(manager);
    expect_requestType(results, request, screenManagerRequestNone, "inactive manager does not create requests from mouse input");

    screenManager_setActive(manager, sfTrue);

    test_check(results, screenManager_isActive(manager) == sfTrue, "screen manager can be reactivated");
}

static void test_screenManagerSetScreen(TestResults* results, screenManager* manager)
{
    print_section("screenManager_setScreen / screenManager_getScreen");

    screenManager_setScreen(manager, screenIdMainMenu);
    expect_screen(results, manager, screenIdMainMenu, "setScreen(MainMenu) works");

    screenManager_setScreen(manager, screenIdSettingsMenu);
    expect_screen(results, manager, screenIdSettingsMenu, "setScreen(SettingsMenu) works");

    screenManager_setScreen(manager, screenIdControlsMenu);
    expect_screen(results, manager, screenIdControlsMenu, "setScreen(ControlsMenu) works");

    screenManager_setScreen(manager, screenIdCreditsMenu);
    expect_screen(results, manager, screenIdCreditsMenu, "setScreen(CreditsMenu) works");

    screenManager_setScreen(manager, screenIdGameSetupMenu);
    expect_screen(results, manager, screenIdGameSetupMenu, "setScreen(GameSetupMenu) works");

    screenManager_setScreen(manager, screenIdLocalPvPSetupMenu);
    expect_screen(results, manager, screenIdLocalPvPSetupMenu, "setScreen(LocalPvPSetupMenu) works");

    screenManager_setScreen(manager, screenIdEnginePvESetupMenu);
    expect_screen(results, manager, screenIdEnginePvESetupMenu, "setScreen(EnginePvESetupMenu) works");

    screenManager_setScreen(manager, screenIdMainMenu);
    expect_screen(results, manager, screenIdMainMenu, "screen restored to main menu");
}

static void test_mainNavigation(TestResults* results, screenManager* manager)
{
    print_section("Main navigation");

    screenManager_setScreen(manager, screenIdMainMenu);
    discard_request(manager);

    simulate_click(manager, mainMenu_point(mainMenuPointPlay), "Main/Play -> Game setup");
    expect_screen(results, manager, screenIdGameSetupMenu, "Play opens game setup menu");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "Play does not create app request directly");

    simulate_click(manager, gameSetupMenu_point(gameSetupMenuPointBack), "Game setup/Back -> Main");
    expect_screen(results, manager, screenIdMainMenu, "Game setup Back returns to main menu");

    simulate_click(manager, mainMenu_point(mainMenuPointSettings), "Main/Settings -> Settings");
    expect_screen(results, manager, screenIdSettingsMenu, "Settings opens settings menu");

    simulate_click(manager, settingsMenu_point(settingsMenuPointBack), "Settings/Back -> Main");
    expect_screen(results, manager, screenIdMainMenu, "Settings Back returns to main menu");

    simulate_click(manager, mainMenu_point(mainMenuPointCredits), "Main/Credits -> Credits");
    expect_screen(results, manager, screenIdCreditsMenu, "Credits opens credits menu");

    simulate_click(manager, simpleBackMenu_point(), "Credits/Back -> Main");
    expect_screen(results, manager, screenIdMainMenu, "Credits Back returns to main menu");
}

static void test_controlsNavigation(TestResults* results, screenManager* manager)
{
    print_section("Controls menu navigation");

    screenManager_setScreen(manager, screenIdMainMenu);
    discard_request(manager);

    simulate_click(manager, mainMenu_point(mainMenuPointSettings), "Main/Settings -> Settings");
    expect_screen(results, manager, screenIdSettingsMenu, "settings menu reached");

    simulate_click(manager, settingsMenu_point(settingsMenuPointControls), "Settings/Controls -> Controls");
    expect_screen(results, manager, screenIdControlsMenu, "Controls button opens controls menu");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "Controls navigation creates no app request");

    simulate_click(manager, simpleBackMenu_point(), "Controls/Back -> Settings");
    expect_screen(results, manager, screenIdSettingsMenu, "Controls Back returns to settings menu");

    simulate_click(manager, settingsMenu_point(settingsMenuPointBack), "Settings/Back -> Main");
    expect_screen(results, manager, screenIdMainMenu, "Settings Back returns to main menu after controls test");
}

static void test_exitRequest(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest firstGet;
    ScreenManagerRequest secondGet;
    ScreenManagerRequest consumed;

    print_section("Exit request");

    screenManager_setScreen(manager, screenIdMainMenu);
    discard_request(manager);

    simulate_click(manager, mainMenu_point(mainMenuPointExit), "Main/Exit -> Exit request");

    expect_screen(results, manager, screenIdMainMenu, "Exit request does not change screen");

    firstGet = screenManager_getRequest(manager);
    secondGet = screenManager_getRequest(manager);

    expect_requestType(results, firstGet, screenManagerRequestExit, "Exit button creates Exit request");
    expect_requestType(results, secondGet, screenManagerRequestExit, "getRequest is non-consuming");

    consumed = screenManager_consumeRequest(manager);

    expect_requestType(results, consumed, screenManagerRequestExit, "consumeRequest returns Exit request");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after consuming Exit");
}

static void test_settingsRequests(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Settings requests");

    screenManager_setScreen(manager, screenIdSettingsMenu);
    discard_request(manager);

    simulate_click(manager, settingsMenu_point(settingsMenuPointSound), "Settings/Sound toggle");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestApplySettings, "Sound toggle creates ApplySettings request");

    if (request.type == screenManagerRequestApplySettings)
    {
        printf(
            "[INFO] Settings data: sound=%s fullscreen=%s\n",
            bool_toString(request.data.settings.soundEnabled),
            bool_toString(request.data.settings.fullscreenEnabled)
        );

        test_check(results, request.data.settings.soundEnabled == sfFalse, "Sound toggles from true to false");
        test_check(results, request.data.settings.fullscreenEnabled == sfFalse, "Sound toggle preserves fullscreen false");
    }

    expect_requestType(results, screenManager_consumeRequest(manager), screenManagerRequestApplySettings, "consumeRequest returns Sound ApplySettings");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after Sound ApplySettings");

    simulate_click(manager, settingsMenu_point(settingsMenuPointFullscreen), "Settings/Fullscreen toggle");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestApplySettings, "Fullscreen toggle creates ApplySettings request");

    if (request.type == screenManagerRequestApplySettings)
    {
        printf(
            "[INFO] Settings data: sound=%s fullscreen=%s\n",
            bool_toString(request.data.settings.soundEnabled),
            bool_toString(request.data.settings.fullscreenEnabled)
        );

        test_check(results, request.data.settings.soundEnabled == sfFalse, "Fullscreen toggle preserves sound false");
        test_check(results, request.data.settings.fullscreenEnabled == sfTrue, "Fullscreen toggles from false to true");
    }

    expect_requestType(results, screenManager_consumeRequest(manager), screenManagerRequestApplySettings, "consumeRequest returns Fullscreen ApplySettings");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after Fullscreen ApplySettings");
}

static void test_gameSetupNavigation(TestResults* results, screenManager* manager)
{
    print_section("Game setup navigation");

    screenManager_setScreen(manager, screenIdGameSetupMenu);
    discard_request(manager);

    simulate_click(manager, gameSetupMenu_point(gameSetupMenuPointLocalPvP), "Game setup/Local PvP");
    expect_screen(results, manager, screenIdLocalPvPSetupMenu, "Local PvP opens Local PvP setup menu");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "Local PvP navigation creates no app request");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointBack), "Local PvP setup/Back");
    expect_screen(results, manager, screenIdGameSetupMenu, "Local PvP Back returns to game setup");

    simulate_click(manager, gameSetupMenu_point(gameSetupMenuPointVsEngine), "Game setup/Vs Engine");
    expect_screen(results, manager, screenIdEnginePvESetupMenu, "Vs Engine opens engine setup menu");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "Vs Engine navigation creates no app request");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointBack), "Engine setup/Back");
    expect_screen(results, manager, screenIdGameSetupMenu, "Engine setup Back returns to game setup");

    simulate_click(manager, gameSetupMenu_point(gameSetupMenuPointBack), "Game setup/Back");
    expect_screen(results, manager, screenIdMainMenu, "Game setup Back returns to main menu");
}

static void test_localPvPDefaultStart(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Local PvP setup default StartGame request");

    screenManager_setScreen(manager, screenIdGameSetupMenu);
    discard_request(manager);

    simulate_click(manager, gameSetupMenu_point(gameSetupMenuPointLocalPvP), "Game setup/Local PvP");
    expect_screen(results, manager, screenIdLocalPvPSetupMenu, "Local PvP setup reached");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointPlay), "Local PvP setup/Play with defaults");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "Local PvP Play creates StartGame request");

    if (request.type == screenManagerRequestStartGame)
    {
        print_requestDetails(request);

        test_check(results, request.data.gameSetup.mode == gameModeLocalPvP, "Local PvP request uses Local PvP mode");
        test_check(results, request.data.gameSetup.data.localPvP.startingTimeSeconds == EXPECTED_DEFAULT_STARTING_TIME_SECONDS, "Local PvP default starting time is 15 minutes");
        test_check(results, request.data.gameSetup.data.localPvP.incrementSeconds == EXPECTED_DEFAULT_INCREMENT_SECONDS, "Local PvP default increment is 0 seconds");
    }

    expect_requestType(results, screenManager_consumeRequest(manager), screenManagerRequestStartGame, "consumeRequest returns Local PvP StartGame");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after Local PvP StartGame");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointBack), "Local PvP setup/Back");
    expect_screen(results, manager, screenIdGameSetupMenu, "Local PvP Back returns to game setup");
}

static void test_localPvPCustomStart(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Local PvP setup custom text-field input");

    screenManager_setScreen(manager, screenIdGameSetupMenu);
    discard_request(manager);

    simulate_click(manager, gameSetupMenu_point(gameSetupMenuPointLocalPvP), "Game setup/Local PvP");
    expect_screen(results, manager, screenIdLocalPvPSetupMenu, "Local PvP setup reached");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointStartingMinutesField), "Select starting minutes field");
    send_backspaces(manager, 2);
    send_text_string(manager, "10");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointIncrementSecondsField), "Select increment seconds field");
    send_backspaces(manager, 1);
    send_text_string(manager, "5");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointPlay), "Local PvP setup/Play with 10+5");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "valid Local PvP custom input creates StartGame request");

    if (request.type == screenManagerRequestStartGame)
    {
        print_requestDetails(request);

        test_check(results, request.data.gameSetup.mode == gameModeLocalPvP, "custom Local PvP request uses Local PvP mode");
        test_check(results, request.data.gameSetup.data.localPvP.startingTimeSeconds == 10 * 60, "custom Local PvP starting time is 10 minutes");
        test_check(results, request.data.gameSetup.data.localPvP.incrementSeconds == 5, "custom Local PvP increment is 5 seconds");
    }

    expect_requestType(results, screenManager_consumeRequest(manager), screenManagerRequestStartGame, "consumeRequest returns custom Local PvP StartGame");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after custom Local PvP StartGame");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointStartingMinutesField), "Select starting minutes field for invalid 0");
    send_backspaces(manager, 2);
    send_text_string(manager, "0");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointPlay), "Local PvP setup/Play with invalid 0+5");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestNone, "invalid Local PvP starting time creates no request");
    expect_screen(results, manager, screenIdLocalPvPSetupMenu, "invalid Local PvP input keeps current screen");

    simulate_click(manager, localPvPSetupMenu_point(localPvPSetupPointBack), "Local PvP setup/Back");
    expect_screen(results, manager, screenIdGameSetupMenu, "Local PvP Back returns to game setup after invalid input test");
}

static void test_enginePvEDefaultStart(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Engine PvE setup default StartGame request");

    screenManager_setScreen(manager, screenIdGameSetupMenu);
    discard_request(manager);

    simulate_click(manager, gameSetupMenu_point(gameSetupMenuPointVsEngine), "Game setup/Vs Engine");
    expect_screen(results, manager, screenIdEnginePvESetupMenu, "Engine PvE setup reached");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointPlay), "Engine setup/Play with defaults");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "Engine PvE Play creates StartGame request");

    if (request.type == screenManagerRequestStartGame)
    {
        print_requestDetails(request);

        test_check(results, request.data.gameSetup.mode == gameModeVsEngine, "Engine request uses Vs Engine mode");
        test_check(results, request.data.gameSetup.data.vsEngine.playerColor == playerColorWhite, "default engine player color is White");
        test_check(results, request.data.gameSetup.data.vsEngine.engineLevel == EXPECTED_DEFAULT_ENGINE_LEVEL, "default engine level is 5");
    }

    expect_requestType(results, screenManager_consumeRequest(manager), screenManagerRequestStartGame, "consumeRequest returns default Engine StartGame");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after default Engine StartGame");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointBack), "Engine setup/Back");
    expect_screen(results, manager, screenIdGameSetupMenu, "Engine Back returns to game setup");
}

static void test_enginePvEColorAndLevel(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Engine PvE setup color cycling and engine level input");

    screenManager_setScreen(manager, screenIdGameSetupMenu);
    discard_request(manager);

    simulate_click(manager, gameSetupMenu_point(gameSetupMenuPointVsEngine), "Game setup/Vs Engine");
    expect_screen(results, manager, screenIdEnginePvESetupMenu, "Engine PvE setup reached");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointColor), "Engine setup/Color: White -> Black");
    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointPlay), "Engine setup/Play with Black");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "Engine PvE Black creates StartGame request");

    if (request.type == screenManagerRequestStartGame)
    {
        print_requestDetails(request);

        test_check(results, request.data.gameSetup.data.vsEngine.playerColor == playerColorBlack, "first color click changes player color to Black");
        test_check(results, request.data.gameSetup.data.vsEngine.engineLevel == EXPECTED_DEFAULT_ENGINE_LEVEL, "engine level remains 5 after color click");
    }

    discard_request(manager);

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointColor), "Engine setup/Color: Black -> Random");
    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointPlay), "Engine setup/Play with Random");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "Engine PvE Random creates StartGame request");

    if (request.type == screenManagerRequestStartGame)
    {
        print_requestDetails(request);

        test_check(results, request.data.gameSetup.data.vsEngine.playerColor == playerColorRandom, "second color click changes player color to Random");
    }

    discard_request(manager);

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointEngineLevelField), "Select engine level field");
    send_backspaces(manager, 1);
    send_text_string(manager, "20");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointPlay), "Engine setup/Play with level 20");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "valid engine level 20 creates StartGame request");

    if (request.type == screenManagerRequestStartGame)
    {
        print_requestDetails(request);

        test_check(results, request.data.gameSetup.mode == gameModeVsEngine, "level 20 request uses Vs Engine mode");
        test_check(results, request.data.gameSetup.data.vsEngine.engineLevel == 20, "engine level was changed to 20");
        test_check(results, request.data.gameSetup.data.vsEngine.playerColor == playerColorRandom, "player color remains Random");
    }

    discard_request(manager);

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointEngineLevelField), "Select engine level field for invalid 21");
    send_backspaces(manager, 2);
    send_text_string(manager, "21");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointPlay), "Engine setup/Play with invalid level 21");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestNone, "invalid engine level 21 creates no request");
    expect_screen(results, manager, screenIdEnginePvESetupMenu, "invalid engine level keeps current screen");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointEngineLevelField), "Select engine level field for invalid letters");
    send_backspaces(manager, 2);
    send_text_string(manager, "abc");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointPlay), "Engine setup/Play with invalid letters");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestNone, "non-numeric engine level creates no request");
    expect_screen(results, manager, screenIdEnginePvESetupMenu, "non-numeric engine level keeps current screen");

    simulate_click(manager, enginePvESetupMenu_point(enginePvESetupPointBack), "Engine setup/Back");
    expect_screen(results, manager, screenIdGameSetupMenu, "Engine Back returns to game setup after input tests");
}

static void print_testSummary(const TestResults* results)
{
    print_section("Test summary");

    printf("Passed: %u\n", results->passed);
    printf("Failed: %u\n", results->failed);

    if (results->failed == 0)
        printf("Result: automated tests passed.\n");
    else
        printf("Result: fix the failed checks before trusting this version.\n");
}

static sfRenderWindow* create_testWindow(void)
{
    sfContextSettings settings;

    settings = (sfContextSettings){0};
    settings.antialiasingLevel = 8;

    return sfRenderWindow_create(
        (sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
        "Chess: Full Project Test",
        sfClose,
        &settings
    );
}

static void run_manualVisualTest(const Resources* resources)
{
    screenManager* manager;
    Mouse* mouse;
    sfRenderWindow* window;
    sfRectangleShape* background;
    sfEvent event;
    ScreenId previousScreen;
    sfContextSettings actualSettings;

    print_section("Manual visual/action test");

    printf("Manual expectations:\n");
    printf("- Main menu starts visible.\n");
    printf("- Play opens Game setup.\n");
    printf("- Game setup opens Local PvP and Vs Engine setup menus.\n");
    printf("- Settings opens Settings, Controls opens Controls, Back returns correctly.\n");
    printf("- Credits opens Credits, Back returns to Main.\n");
    printf("- Local PvP accepts numeric text input and rejects invalid input.\n");
    printf("- Engine PvE cycles color and accepts engine level 0-20.\n");
    printf("- Exit closes the window.\n");
    printf("- Escape also closes the window.\n\n");

    window = create_testWindow();

    if (window == NULL)
    {
        printf("[FAIL] Could not create render window for manual visual test.\n");
        return;
    }

    actualSettings = sfRenderWindow_getSettings(window);

    printf("[INFO] Actual antialiasing level: %u\n", actualSettings.antialiasingLevel);

    sfRenderWindow_setFramerateLimit(window, 60);

    background = sfRectangleShape_create();

    if (background != NULL)
    {
        sfRectangleShape_setSize(background, (sfVector2f){WINDOW_WIDTH, WINDOW_HEIGHT});
        sfRectangleShape_setTexture(background, resources_getTexture(resources, resourceTextureBackground), sfTrue);
    }

    manager = screenManager_create(
        (sfVector2i){TEST_TOP_LEFT_X, TEST_TOP_LEFT_Y},
        (sfVector2i){TEST_BOTTOM_RIGHT_X, TEST_BOTTOM_RIGHT_Y},
        resources
    );

    if (manager == NULL)
    {
        printf("[FAIL] Could not create screen manager for manual visual test.\n");

        if (background != NULL)
            sfRectangleShape_destroy(background);

        sfRenderWindow_destroy(window);
        return;
    }

    mouse = mouse_create(sfMouseLeft);

    if (mouse == NULL)
    {
        printf("[FAIL] Could not create mouse object for manual visual test.\n");

        screenManager_destroy(manager);

        if (background != NULL)
            sfRectangleShape_destroy(background);

        sfRenderWindow_destroy(window);
        return;
    }

    previousScreen = screenManager_getScreen(manager);
    printf("[SCREEN] %s\n", screenId_toString(previousScreen));

    while (sfRenderWindow_isOpen(window))
    {
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);

            if (event.type == sfEvtKeyReleased && event.key.code == sfKeyEscape)
                sfRenderWindow_close(window);

            screenManager_updateKeyboard(manager, &event);
        }

        mouse_update(mouse, window);
        screenManager_updateMouse(manager, mouse);

        if (screenManager_getScreen(manager) != previousScreen)
        {
            previousScreen = screenManager_getScreen(manager);
            printf("[SCREEN] %s\n", screenId_toString(previousScreen));
        }

        {
            ScreenManagerRequest request;

            request = screenManager_consumeRequest(manager);

            if (request.type != screenManagerRequestNone)
            {
                print_requestDetails(request);

                if (request.type == screenManagerRequestExit)
                    sfRenderWindow_close(window);
            }
        }

        sfRenderWindow_clear(window, sfBlack);

        if (background != NULL)
            sfRenderWindow_drawRectangleShape(window, background, NULL);

        screenManager_draw(window, manager);

        sfRenderWindow_display(window);
    }

    mouse_destroy(mouse);
    screenManager_destroy(manager);

    if (background != NULL)
        sfRectangleShape_destroy(background);

    sfRenderWindow_destroy(window);
}

int main(void)
{
    TestResults results;
    Resources* resources;
    screenManager* manager;

    results = (TestResults){0, 0};

    resources_destroy(NULL);

    test_mouse(&results);
    test_screenDataDefaults(&results);
    test_requestHelpers(&results);
    test_screenManagerNullSafety(&results);

    resources = resources_create();

    test_resources(&results, resources);

    if (resources == NULL)
    {
        print_testSummary(&results);
        return 1;
    }

    manager = screenManager_create(
        (sfVector2i){TEST_TOP_LEFT_X, TEST_TOP_LEFT_Y},
        (sfVector2i){TEST_BOTTOM_RIGHT_X, TEST_BOTTOM_RIGHT_Y},
        resources
    );

    test_screenManagerCreation(&results, manager);

    if (manager != NULL)
    {
        test_screenManagerActiveState(&results, manager);
        test_screenManagerSetScreen(&results, manager);
        test_mainNavigation(&results, manager);
        test_controlsNavigation(&results, manager);
        test_exitRequest(&results, manager);
        test_settingsRequests(&results, manager);
        test_gameSetupNavigation(&results, manager);
        test_localPvPDefaultStart(&results, manager);
        test_localPvPCustomStart(&results, manager);
        test_enginePvEDefaultStart(&results, manager);
        test_enginePvEColorAndLevel(&results, manager);

        screenManager_destroy(manager);
    }

    print_testSummary(&results);

    run_manualVisualTest(resources);

    resources_destroy(resources);

    return results.failed == 0 ? 0 : 1;
}