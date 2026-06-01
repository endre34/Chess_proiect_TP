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

#define EXPECTED_DEFAULT_TIME_SECONDS 600
#define EXPECTED_DEFAULT_ENGINE_LEVEL 5

typedef struct TestResults
{
    unsigned int passed;
    unsigned int failed;

} TestResults;

typedef enum MainMenuButtonPoint
{
    mainMenuPointPlay,
    mainMenuPointSettings,
    mainMenuPointCredits,
    mainMenuPointExit

} MainMenuButtonPoint;

typedef enum SettingsMenuButtonPoint
{
    settingsMenuPointSound,
    settingsMenuPointFullscreen,
    settingsMenuPointControls,
    settingsMenuPointBack

} SettingsMenuButtonPoint;

typedef enum GameSetupMenuButtonPoint
{
    gameSetupMenuPointLocalPvP,
    gameSetupMenuPointVsEngine,
    gameSetupMenuPointBack

} GameSetupMenuButtonPoint;

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
    if (value)
        return "true";

    return "false";
}

static const char* screenId_toString(ScreenId screen)
{
    switch (screen)
    {
        case screenIdMainMenu:
            return "Main menu";

        case screenIdSettingsMenu:
            return "Settings menu";

        case screenIdCreditsMenu:
            return "Credits menu";

        case screenIdGameSetupMenu:
            return "Game setup menu";

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

static float test_areaWidth(void)
{
    return (float)(TEST_BOTTOM_RIGHT_X - TEST_TOP_LEFT_X);
}

static float test_areaHeight(void)
{
    return (float)(TEST_BOTTOM_RIGHT_Y - TEST_TOP_LEFT_Y);
}

static float test_centerX(void)
{
    return (float)TEST_TOP_LEFT_X + test_areaWidth() / 2.0f;
}

static sfVector2i mainMenu_buttonPoint(MainMenuButtonPoint point)
{
    float firstButtonY;
    float buttonGap;

    firstButtonY = (float)TEST_TOP_LEFT_Y + test_areaHeight() * 0.40f;
    buttonGap = test_areaHeight() * 0.115f;

    switch (point)
    {
        case mainMenuPointPlay:
            return make_point(test_centerX(), firstButtonY);

        case mainMenuPointSettings:
            return make_point(test_centerX(), firstButtonY + buttonGap);

        case mainMenuPointCredits:
            return make_point(test_centerX(), firstButtonY + 2.0f * buttonGap);

        case mainMenuPointExit:
            return make_point(test_centerX(), firstButtonY + 3.0f * buttonGap);

        default:
            return make_point(0.0f, 0.0f);
    }
}

static sfVector2i settingsMenu_buttonPoint(SettingsMenuButtonPoint point)
{
    float firstButtonY;
    float buttonGap;

    firstButtonY = (float)TEST_TOP_LEFT_Y + test_areaHeight() * 0.53f;
    buttonGap = test_areaHeight() * 0.11f;

    switch (point)
    {
        case settingsMenuPointSound:
            return make_point(test_centerX(), firstButtonY);

        case settingsMenuPointFullscreen:
            return make_point(test_centerX(), firstButtonY + buttonGap);

        case settingsMenuPointControls:
            return make_point(test_centerX(), firstButtonY + 2.0f * buttonGap);

        case settingsMenuPointBack:
            return make_point(test_centerX(), firstButtonY + 3.0f * buttonGap);

        default:
            return make_point(0.0f, 0.0f);
    }
}

static sfVector2i creditsMenu_backPoint(void)
{
    return make_point(
        test_centerX(),
        (float)TEST_TOP_LEFT_Y + test_areaHeight() * 0.80f
    );
}

static sfVector2i gameSetupMenu_buttonPoint(GameSetupMenuButtonPoint point)
{
    float firstButtonY;
    float buttonGap;

    firstButtonY = (float)TEST_TOP_LEFT_Y + test_areaHeight() * 0.50f;
    buttonGap = test_areaHeight() * 0.11f;

    switch (point)
    {
        case gameSetupMenuPointLocalPvP:
            return make_point(test_centerX(), firstButtonY);

        case gameSetupMenuPointVsEngine:
            return make_point(test_centerX(), firstButtonY + buttonGap);

        case gameSetupMenuPointBack:
            return make_point(test_centerX(), firstButtonY + 3.0f * buttonGap);

        default:
            return make_point(0.0f, 0.0f);
    }
}

static void print_click(const char* label, sfVector2i point)
{
    printf("[CLICK] %-32s at (%d, %d)\n", label, point.x, point.y);
}

static Mouse make_testMouse(sfVector2i position, sfBool pressed, sfBool justPressed, sfBool justReleased)
{
    Mouse mouse;

    mouse.button = sfMouseLeft;
    mouse.position = position;
    mouse.pressed = pressed;
    mouse.justPressed = justPressed;
    mouse.justReleased = justReleased;

    return mouse;
}

static void simulate_click(screenManager* manager, sfVector2i point, const char* label)
{
    Mouse pressMouse;
    Mouse releaseMouse;

    print_click(label, point);

    pressMouse = make_testMouse(point, sfTrue, sfTrue, sfFalse);
    screenManager_updateMouse(manager, &pressMouse);

    releaseMouse = make_testMouse(point, sfFalse, sfFalse, sfTrue);
    screenManager_updateMouse(manager, &releaseMouse);
}

static void expect_screen(TestResults* results, const screenManager* manager, ScreenId expected, const char* message)
{
    ScreenId actual;

    actual = screenManager_getScreen(manager);

    printf(
        "[INFO] Current screen: expected=%s, actual=%s\n",
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

static void test_nullSafety(TestResults* results)
{
    ScreenManagerRequest request;

    print_section("Null safety and default return values");

    test_check(
        results,
        screenManager_create(
            (sfVector2i){TEST_TOP_LEFT_X, TEST_TOP_LEFT_Y},
            (sfVector2i){TEST_BOTTOM_RIGHT_X, TEST_BOTTOM_RIGHT_Y},
            NULL
        ) == NULL,
        "screenManager_create returns NULL when Resources is NULL"
    );

    screenManager_setActive(NULL, sfTrue);
    screenManager_setScreen(NULL, screenIdCreditsMenu);
    screenManager_updateMouse(NULL, NULL);
    screenManager_draw(NULL, NULL);
    screenManager_destroy(NULL);

    test_check(
        results,
        screenManager_isActive(NULL) == sfFalse,
        "screenManager_isActive(NULL) returns false"
    );

    test_check(
        results,
        screenManager_getScreen(NULL) == screenIdMainMenu,
        "screenManager_getScreen(NULL) returns main menu fallback"
    );

    request = screenManager_getRequest(NULL);

    test_check(
        results,
        request.type == screenManagerRequestNone,
        "screenManager_getRequest(NULL) returns none"
    );

    request = screenManager_consumeRequest(NULL);

    test_check(
        results,
        request.type == screenManagerRequestNone,
        "screenManager_consumeRequest(NULL) returns none"
    );

    printf("[INFO] Null calls completed without crashing.\n");
}

static void test_screenDataDefaults(TestResults* results)
{
    SettingsData settings;
    LocalPvPSetup localPvP;
    VsEngineSetup vsEngine;
    GameSetupData gameSetup;

    print_section("Screen data defaults used by screen manager requests");

    settings = settingsData_getDefault();
    localPvP = localPvPSetup_getDefault();
    vsEngine = vsEngineSetup_getDefault();
    gameSetup = gameSetupData_getDefault();

    test_check(results, settings.soundEnabled == sfTrue, "default settings: sound starts enabled");
    test_check(results, settings.fullscreenEnabled == sfFalse, "default settings: fullscreen starts disabled");

    test_check(results, localPvP.whiteTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "default Local PvP: white time is 600 seconds");
    test_check(results, localPvP.blackTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "default Local PvP: black time is 600 seconds");

    test_check(results, vsEngine.playerColor == playerColorWhite, "default Vs Engine: player color is white");
    test_check(results, vsEngine.engineLevel == EXPECTED_DEFAULT_ENGINE_LEVEL, "default Vs Engine: engine level is 5");
    test_check(results, vsEngine.playerTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "default Vs Engine: player time is 600 seconds");
    test_check(results, vsEngine.engineTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "default Vs Engine: engine time is 600 seconds");

    test_check(results, gameSetup.mode == gameModeLocalPvP, "default GameSetupData mode is Local PvP");
    test_check(results, gameSetup.data.localPvP.whiteTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "default GameSetupData local PvP white time is 600 seconds");
    test_check(results, gameSetup.data.localPvP.blackTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "default GameSetupData local PvP black time is 600 seconds");
}

static void test_resourceSanity(TestResults* results, const Resources* resources)
{
    print_section("Resource sanity required before screen manager creation");

    test_check(results, resources != NULL, "resources object exists");

    if (resources == NULL)
        return;

    test_check(results, resources_getTexture(resources, resourceTextureBackground) != NULL, "background texture loaded");
    test_check(results, resources_getTexture(resources, resourceTextureButtons) != NULL, "button texture loaded");
    test_check(results, resources_getTexture(resources, resourceTextureTitleBoard) != NULL, "title board texture loaded");

    test_check(results, resources_getFont(resources, resourceFontCinzelMedium) != NULL, "main menu button font loaded");
    test_check(results, resources_getFont(resources, resourceFontCinzelSemiBold) != NULL, "title font loaded");
    test_check(results, resources_getFont(resources, resourceFontJetBrainsMonoRegular) != NULL, "info text font loaded");
}

static void test_creationAndBasicState(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Screen manager creation and initial state");

    test_check(results, manager != NULL, "screen manager object created");

    if (manager == NULL)
        return;

    test_check(results, screenManager_isActive(manager) == sfTrue, "screen manager starts active");
    expect_screen(results, manager, screenIdMainMenu, "screen manager starts on the main menu");

    request = screenManager_getRequest(manager);
    expect_requestType(results, request, screenManagerRequestNone, "screen manager starts with no pending request");

    request = screenManager_consumeRequest(manager);
    expect_requestType(results, request, screenManagerRequestNone, "consumeRequest also returns none initially");

    request = screenManager_getRequest(manager);
    expect_requestType(results, request, screenManagerRequestNone, "request remains none after consuming initial request");
}

static void test_activeState(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Active state");

    screenManager_setActive(manager, sfFalse);
    test_check(results, screenManager_isActive(manager) == sfFalse, "screen manager can be deactivated");

    screenManager_setScreen(manager, screenIdMainMenu);

    simulate_click(
        manager,
        mainMenu_buttonPoint(mainMenuPointPlay),
        "inactive manager: main Play should be ignored"
    );

    expect_screen(results, manager, screenIdMainMenu, "inactive manager ignores mouse updates and stays on main menu");

    request = screenManager_getRequest(manager);
    expect_requestType(results, request, screenManagerRequestNone, "inactive manager does not create a request from a click");

    screenManager_setActive(manager, sfTrue);
    test_check(results, screenManager_isActive(manager) == sfTrue, "screen manager can be reactivated");
}

static void test_explicitScreenSetter(TestResults* results, screenManager* manager)
{
    print_section("Explicit screen setter and getter");

    screenManager_setScreen(manager, screenIdMainMenu);
    expect_screen(results, manager, screenIdMainMenu, "setScreen(MainMenu) works");

    screenManager_setScreen(manager, screenIdSettingsMenu);
    expect_screen(results, manager, screenIdSettingsMenu, "setScreen(SettingsMenu) works");

    screenManager_setScreen(manager, screenIdCreditsMenu);
    expect_screen(results, manager, screenIdCreditsMenu, "setScreen(CreditsMenu) works");

    screenManager_setScreen(manager, screenIdGameSetupMenu);
    expect_screen(results, manager, screenIdGameSetupMenu, "setScreen(GameSetupMenu) works");

    screenManager_setScreen(manager, screenIdMainMenu);
    expect_screen(results, manager, screenIdMainMenu, "screen restored to main menu after setter tests");
}

static void test_mainMenuNavigation(TestResults* results, screenManager* manager)
{
    print_section("Main menu navigation through screen manager");

    screenManager_setScreen(manager, screenIdMainMenu);

    simulate_click(manager, mainMenu_buttonPoint(mainMenuPointPlay), "main Play -> Game setup");
    expect_screen(results, manager, screenIdGameSetupMenu, "main Play switches to game setup menu");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "main Play does not create an app request yet");

    simulate_click(manager, gameSetupMenu_buttonPoint(gameSetupMenuPointBack), "game setup Back -> Main");
    expect_screen(results, manager, screenIdMainMenu, "game setup Back returns to main menu");

    simulate_click(manager, mainMenu_buttonPoint(mainMenuPointSettings), "main Settings -> Settings");
    expect_screen(results, manager, screenIdSettingsMenu, "main Settings switches to settings menu");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "main Settings does not create an app request");

    simulate_click(manager, settingsMenu_buttonPoint(settingsMenuPointBack), "settings Back -> Main");
    expect_screen(results, manager, screenIdMainMenu, "settings Back returns to main menu");

    simulate_click(manager, mainMenu_buttonPoint(mainMenuPointCredits), "main Credits -> Credits");
    expect_screen(results, manager, screenIdCreditsMenu, "main Credits switches to credits menu");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "main Credits does not create an app request");

    simulate_click(manager, creditsMenu_backPoint(), "credits Back -> Main");
    expect_screen(results, manager, screenIdMainMenu, "credits Back returns to main menu");
}

static void test_exitRequest(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest requestA;
    ScreenManagerRequest requestB;
    ScreenManagerRequest consumed;

    print_section("Exit request generation and consumption");

    screenManager_setScreen(manager, screenIdMainMenu);

    simulate_click(manager, mainMenu_buttonPoint(mainMenuPointExit), "main Exit -> request Exit");

    expect_screen(results, manager, screenIdMainMenu, "Exit request does not change the current screen");

    requestA = screenManager_getRequest(manager);
    requestB = screenManager_getRequest(manager);

    expect_requestType(results, requestA, screenManagerRequestExit, "main Exit creates an exit request");
    expect_requestType(results, requestB, screenManagerRequestExit, "getRequest is non-consuming");

    consumed = screenManager_consumeRequest(manager);
    expect_requestType(results, consumed, screenManagerRequestExit, "consumeRequest returns the pending exit request");

    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets to none after consuming exit request");
}

static void test_settingsRequests(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Settings menu requests through screen manager");

    screenManager_setScreen(manager, screenIdSettingsMenu);
    expect_screen(results, manager, screenIdSettingsMenu, "settings request tests start on settings menu");

    simulate_click(manager, settingsMenu_buttonPoint(settingsMenuPointControls), "settings Controls -> intentionally no request");

    expect_screen(results, manager, screenIdSettingsMenu, "Controls currently keeps the user on settings menu");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "Controls currently creates no request");

    simulate_click(manager, settingsMenu_buttonPoint(settingsMenuPointSound), "settings Sound -> ApplySettings");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestApplySettings, "Sound toggle creates ApplySettings request");

    if (request.type == screenManagerRequestApplySettings)
    {
        printf(
            "[INFO] Settings data after Sound click: sound=%s, fullscreen=%s\n",
            bool_toString(request.data.settings.soundEnabled),
            bool_toString(request.data.settings.fullscreenEnabled)
        );

        test_check(results, request.data.settings.soundEnabled == sfFalse, "Sound click toggles sound from true to false");
        test_check(results, request.data.settings.fullscreenEnabled == sfFalse, "Sound click preserves fullscreen as false");
    }

    request = screenManager_consumeRequest(manager);

    expect_requestType(results, request, screenManagerRequestApplySettings, "consumeRequest returns ApplySettings after Sound click");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after consuming Sound ApplySettings");

    simulate_click(manager, settingsMenu_buttonPoint(settingsMenuPointFullscreen), "settings Fullscreen -> ApplySettings");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestApplySettings, "Fullscreen toggle creates ApplySettings request");

    if (request.type == screenManagerRequestApplySettings)
    {
        printf(
            "[INFO] Settings data after Fullscreen click: sound=%s, fullscreen=%s\n",
            bool_toString(request.data.settings.soundEnabled),
            bool_toString(request.data.settings.fullscreenEnabled)
        );

        test_check(results, request.data.settings.soundEnabled == sfFalse, "Fullscreen click preserves sound as false");
        test_check(results, request.data.settings.fullscreenEnabled == sfTrue, "Fullscreen click toggles fullscreen from false to true");
    }

    request = screenManager_consumeRequest(manager);

    expect_requestType(results, request, screenManagerRequestApplySettings, "consumeRequest returns ApplySettings after Fullscreen click");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after consuming Fullscreen ApplySettings");

    simulate_click(manager, settingsMenu_buttonPoint(settingsMenuPointBack), "settings Back -> Main");

    expect_screen(results, manager, screenIdMainMenu, "settings Back returns to main menu after request tests");
}

static void test_gameSetupRequests(TestResults* results, screenManager* manager)
{
    ScreenManagerRequest request;

    print_section("Game setup requests through screen manager");

    screenManager_setScreen(manager, screenIdGameSetupMenu);
    expect_screen(results, manager, screenIdGameSetupMenu, "game setup request tests start on game setup menu");

    simulate_click(manager, gameSetupMenu_buttonPoint(gameSetupMenuPointLocalPvP), "game setup Local PvP -> StartGame");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "Local PvP creates StartGame request");

    if (request.type == screenManagerRequestStartGame)
    {
        printf("[INFO] Game setup data: mode=%s\n", gameMode_toString(request.data.gameSetup.mode));

        test_check(results, request.data.gameSetup.mode == gameModeLocalPvP, "Local PvP request uses Local PvP mode");
        test_check(results, request.data.gameSetup.data.localPvP.whiteTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "Local PvP request white time is 600 seconds");
        test_check(results, request.data.gameSetup.data.localPvP.blackTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "Local PvP request black time is 600 seconds");
    }

    request = screenManager_consumeRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "consumeRequest returns StartGame after Local PvP click");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after consuming Local PvP StartGame");

    simulate_click(manager, gameSetupMenu_buttonPoint(gameSetupMenuPointVsEngine), "game setup Vs Engine -> StartGame");

    request = screenManager_getRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "Vs Engine creates StartGame request");

    if (request.type == screenManagerRequestStartGame)
    {
        printf(
            "[INFO] Game setup data: mode=%s, playerColor=%s, engineLevel=%d\n",
            gameMode_toString(request.data.gameSetup.mode),
            playerColor_toString(request.data.gameSetup.data.vsEngine.playerColor),
            request.data.gameSetup.data.vsEngine.engineLevel
        );

        test_check(results, request.data.gameSetup.mode == gameModeVsEngine, "Vs Engine request uses Vs Engine mode");
        test_check(results, request.data.gameSetup.data.vsEngine.playerColor == playerColorWhite, "Vs Engine request default player color is white");
        test_check(results, request.data.gameSetup.data.vsEngine.engineLevel == EXPECTED_DEFAULT_ENGINE_LEVEL, "Vs Engine request engine level is 5");
        test_check(results, request.data.gameSetup.data.vsEngine.playerTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "Vs Engine request player time is 600 seconds");
        test_check(results, request.data.gameSetup.data.vsEngine.engineTimeSeconds == EXPECTED_DEFAULT_TIME_SECONDS, "Vs Engine request engine time is 600 seconds");
    }

    request = screenManager_consumeRequest(manager);

    expect_requestType(results, request, screenManagerRequestStartGame, "consumeRequest returns StartGame after Vs Engine click");
    expect_requestType(results, screenManager_getRequest(manager), screenManagerRequestNone, "request resets after consuming Vs Engine StartGame");

    simulate_click(manager, gameSetupMenu_buttonPoint(gameSetupMenuPointBack), "game setup Back -> Main");

    expect_screen(results, manager, screenIdMainMenu, "game setup Back returns to main menu after request tests");
}

static void print_testSummary(const TestResults* results)
{
    print_section("Test summary");

    printf("Passed: %u\n", results->passed);
    printf("Failed: %u\n", results->failed);

    if (results->failed == 0)
        printf("Result: screen manager tests passed.\n");
    else
        printf("Result: fix the failed checks before trusting the screen manager.\n");
}

static void print_manualInstructions(void)
{
    print_section("Manual visual/action test");

    printf("Window expectations:\n");
    printf("- starts on Main menu\n");
    printf("- Play opens Game setup\n");
    printf("- Settings opens Settings menu\n");
    printf("- Credits opens Credits menu\n");
    printf("- Back returns to Main menu\n");
    printf("- Sound and Fullscreen produce ApplySettings requests\n");
    printf("- Local PvP and Vs Engine produce StartGame requests\n");
    printf("- Exit produces an Exit request and closes the window\n");
    printf("- Escape also closes the window\n\n");
}

static void print_requestDetails(ScreenManagerRequest request)
{
    printf("[REQUEST] %s\n", requestType_toString(request.type));

    switch (request.type)
    {
        case screenManagerRequestApplySettings:
            printf(
                "          sound=%s, fullscreen=%s\n",
                bool_toString(request.data.settings.soundEnabled),
                bool_toString(request.data.settings.fullscreenEnabled)
            );
            break;

        case screenManagerRequestStartGame:
            printf("          mode=%s\n", gameMode_toString(request.data.gameSetup.mode));

            if (request.data.gameSetup.mode == gameModeLocalPvP)
            {
                printf(
                    "          whiteTime=%u, blackTime=%u\n",
                    request.data.gameSetup.data.localPvP.whiteTimeSeconds,
                    request.data.gameSetup.data.localPvP.blackTimeSeconds
                );
            }
            else if (request.data.gameSetup.mode == gameModeVsEngine)
            {
                printf(
                    "          playerColor=%s, engineLevel=%d, playerTime=%u, engineTime=%u\n",
                    playerColor_toString(request.data.gameSetup.data.vsEngine.playerColor),
                    request.data.gameSetup.data.vsEngine.engineLevel,
                    request.data.gameSetup.data.vsEngine.playerTimeSeconds,
                    request.data.gameSetup.data.vsEngine.engineTimeSeconds
                );
            }

            break;

        default:
            break;
    }
}

static sfRenderWindow* create_testWindow(void)
{
    sfContextSettings settings = {0};

    settings.antialiasingLevel = 8;

    return sfRenderWindow_create(
        (sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
        "Chess: Screen Manager Test",
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

    window = create_testWindow();

    if (window == NULL)
    {
        printf("[FAIL] Could not create render window for manual visual test.\n");
        return;
    }

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
    TestResults results = {0, 0};
    Resources* resources;
    screenManager* manager;

    test_nullSafety(&results);
    test_screenDataDefaults(&results);

    resources = resources_create();
    test_resourceSanity(&results, resources);

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

    test_creationAndBasicState(&results, manager);

    if (manager != NULL)
    {
        test_activeState(&results, manager);
        test_explicitScreenSetter(&results, manager);
        test_mainMenuNavigation(&results, manager);
        test_exitRequest(&results, manager);
        test_settingsRequests(&results, manager);
        test_gameSetupRequests(&results, manager);

        screenManager_destroy(manager);
    }

    print_testSummary(&results);
    print_manualInstructions();

    run_manualVisualTest(resources);

    resources_destroy(resources);

    return results.failed == 0 ? 0 : 1;
}