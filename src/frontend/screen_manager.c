#include "frontend/screen_manager.h"

#include "frontend/screens/main_menu.h"
#include "frontend/screens/menus.h"
#include "frontend/screens/settings_menu.h"
#include "frontend/screens/controls_menu.h"
#include "frontend/screens/credits_menu.h"
#include "frontend/screens/game_setup_menu.h"
#include "frontend/screens/local_pvp_setup_menu.h"
#include "frontend/screens/engine_pve_setup_menu.h"

#include <stdlib.h>

struct screenManager
{
    mainMenu* main;
    settingsMenu* settings;
    controlsMenu* controls;
    creditsMenu* credits;

    gameSetupMenu* gameSetup;
    localPvPSetupMenu* localPvPSetup;
    enginePvESetupMenu* enginePvESetup;

    ScreenId currentScreen;
    ScreenManagerRequest request;

    sfBool active;
};

static void screenManager_setOnlyActiveScreen(screenManager*, ScreenId);

static void screenManager_updateMainMenu(screenManager*, const Mouse*);
static void screenManager_updateSettingsMenu(screenManager*, const Mouse*);
static void screenManager_updateControlsMenu(screenManager*, const Mouse*);
static void screenManager_updateCreditsMenu(screenManager*, const Mouse*);

static void screenManager_updateGameSetupMenu(screenManager*, const Mouse*);
static void screenManager_updateLocalPvPSetupMenu(screenManager*, const Mouse*);
static void screenManager_updateEnginePvESetupMenu(screenManager*, const Mouse*);

static void screenManager_setOnlyActiveScreen(screenManager* manager, ScreenId screen)
{
    mainMenu_setActive(manager->main, sfFalse);
    settingsMenu_setActive(manager->settings, sfFalse);
    controlsMenu_setActive(manager->controls, sfFalse);
    creditsMenu_setActive(manager->credits, sfFalse);
    gameSetupMenu_setActive(manager->gameSetup, sfFalse);

    manager->currentScreen = screen;

    switch (screen)
    {
        case screenIdMainMenu:
            mainMenu_setActive(manager->main, sfTrue);
            break;

        case screenIdSettingsMenu:
            settingsMenu_setActive(manager->settings, sfTrue);
            break;

        case screenIdControlsMenu:
            controlsMenu_setActive(manager->controls, sfTrue);
            break;

        case screenIdCreditsMenu:
            creditsMenu_setActive(manager->credits, sfTrue);
            break;

        case screenIdGameSetupMenu:
            gameSetupMenu_setActive(manager->gameSetup, sfTrue);
            break;

        case screenIdLocalPvPSetupMenu:
            localPvPSetupMenu_setActive(manager->localPvPSetup, sfTrue);
            break;

        case screenIdEnginePvESetupMenu:
            enginePvESetupMenu_setActive(manager->enginePvESetup, sfTrue);
            break;
    }
}

static void screenManager_updateMainMenu(screenManager* manager, const Mouse* mouse)
{
    MainMenuAction action;

    mainMenu_updateMouse(manager->main, mouse);

    action = mainMenu_consumeAction(manager->main);

    switch (action)
    {
        case mainMenuActionNone:
            break;

        case mainMenuActionPlay:
            screenManager_setOnlyActiveScreen(manager, screenIdGameSetupMenu);
            break;

        case mainMenuActionSettings:
            screenManager_setOnlyActiveScreen(manager, screenIdSettingsMenu);
            break;

        case mainMenuActionCredits:
            screenManager_setOnlyActiveScreen(manager, screenIdCreditsMenu);
            break;

        case mainMenuActionExit:
            manager->request.type = screenManagerRequestExit;
            break;
    }
}

static void screenManager_updateSettingsMenu(screenManager* manager, const Mouse* mouse)
{
    SettingsMenuAction action;

    settingsMenu_updateMouse(manager->settings, mouse);

    action = settingsMenu_consumeAction(manager->settings);

    switch (action)
    {
        case settingsMenuActionNone:
            break;

        case settingsMenuActionApply:
            manager->request.type = screenManagerRequestApplySettings;
            manager->request.data.settings = settingsMenu_getData(manager->settings);
            break;

        case settingsMenuActionControls:
            screenManager_setOnlyActiveScreen(manager, screenIdControlsMenu);
            break;

        case settingsMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);
            break;
    }
}

static void screenManager_updateControlsMenu(screenManager* manager, const Mouse* mouse)
{
    ControlsMenuAction action;

    controlsMenu_updateMouse(manager->controls, mouse);

    action = controlsMenu_consumeAction(manager->controls);

    switch (action)
    {
        case controlsMenuActionNone:
            break;

        case controlsMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdSettingsMenu);
            break;
    }
}

static void screenManager_updateCreditsMenu(screenManager* manager, const Mouse* mouse)
{
    CreditsMenuAction action;

    creditsMenu_updateMouse(manager->credits, mouse);

    action = creditsMenu_consumeAction(manager->credits);

    switch (action)
    {
        case creditsMenuActionNone:
            break;

        case creditsMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);
            break;
    }
}

static void screenManager_updateGameSetupMenu(screenManager* manager, const Mouse* mouse)
{
    GameSetupMenuAction action;

    gameSetupMenu_updateMouse(manager->gameSetup, mouse);

    action = gameSetupMenu_consumeAction(manager->gameSetup);

    switch (action)
    {
        case gameSetupMenuActionNone:
            break;

        case gameSetupMenuActionLocalPvP:
            screenManager_setOnlyActiveScreen(manager, screenIdLocalPvPSetupMenu);
            break;

        case gameSetupMenuActionVsEngine:
            screenManager_setOnlyActiveScreen(manager, screenIdEnginePvESetupMenu);
            break;

        case gameSetupMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);
            break;
    }
}

static void screenManager_updateLocalPvPSetupMenu(screenManager* manager, const Mouse* mouse)
{
    LocalPvPSetupMenuAction action;
    LocalPvPSetup setup;

    localPvPSetupMenu_updateMouse(manager->localPvPSetup, mouse);

    action = localPvPSetupMenu_consumeAction(manager->localPvPSetup);

    switch (action)
    {
        case localPvPSetupMenuActionNone:
            break;

        case localPvPSetupMenuActionStart:
            setup = localPvPSetupMenu_getData(manager->localPvPSetup);

            manager->request.type = screenManagerRequestStartGame;
            manager->request.data.gameSetup = gameSetupData_makeLocalPvP(setup);
            break;

        case localPvPSetupMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdGameSetupMenu);
            break;
    }
}

static void screenManager_updateEnginePvESetupMenu(screenManager* manager, const Mouse* mouse)
{
    EnginePvESetupMenuAction action;
    VsEngineSetup setup;

    enginePvESetupMenu_updateMouse(manager->enginePvESetup, mouse);

    action = enginePvESetupMenu_consumeAction(manager->enginePvESetup);

    switch (action)
    {
        case enginePvESetupMenuActionNone:
            break;

        case enginePvESetupMenuActionStart:
            setup = enginePvESetupMenu_getData(manager->enginePvESetup);

            manager->request.type = screenManagerRequestStartGame;
            manager->request.data.gameSetup = gameSetupData_makeVsEngine(setup);
            break;

        case enginePvESetupMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdGameSetupMenu);
            break;
    }
}

screenManager* screenManager_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    screenManager* manager;

    if (resources == NULL)
        return NULL;

    manager = malloc(sizeof(screenManager));

    if (manager == NULL)
        return NULL;

    manager->main = NULL;
    manager->settings = NULL;
    manager->controls = NULL;
    manager->credits = NULL;
    manager->gameSetup = NULL;
    manager->localPvPSetup = NULL;
    manager->enginePvESetup = NULL;

    manager->currentScreen = screenIdMainMenu;
    manager->request = screenManagerRequest_getNone();
    manager->active = sfTrue;

    manager->main = mainMenu_create(topLeft, bottomRight, resources);
    manager->settings = settingsMenu_create(topLeft, bottomRight, resources);
    manager->controls = controlsMenu_create(topLeft, bottomRight, resources);
    manager->credits = creditsMenu_create(topLeft, bottomRight, resources);
    manager->gameSetup = gameSetupMenu_create(topLeft, bottomRight, resources);
    manager->localPvPSetup = localPvPSetupMenu_create(topLeft, bottomRight, resources);
    manager->enginePvESetup = enginePvESetupMenu_create(topLeft, bottomRight, resources);

    if (
    manager->main == NULL ||
    manager->settings == NULL ||
    manager->controls == NULL ||
    manager->credits == NULL ||
    manager->gameSetup == NULL ||
    manager->localPvPSetup == NULL ||
    manager->enginePvESetup == NULL
    )
    {
        screenManager_destroy(manager);
        return NULL;
    }

    screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);

    return manager;
}

void screenManager_destroy(screenManager* manager)
{
    if (manager == NULL)
        return;

    if (manager->main != NULL)
        mainMenu_destroy(manager->main);

    if (manager->settings != NULL)
        settingsMenu_destroy(manager->settings);

    if (manager->controls != NULL)
        controlsMenu_destroy(manager->controls);

    if (manager->credits != NULL)
        creditsMenu_destroy(manager->credits);

    if (manager->gameSetup != NULL)
        gameSetupMenu_destroy(manager->gameSetup);

    if (manager->localPvPSetup != NULL)
        localPvPSetupMenu_destroy(manager->localPvPSetup);

    if (manager->enginePvESetup != NULL)
        enginePvESetupMenu_destroy(manager->enginePvESetup);

    free(manager);
}

void screenManager_updateMouse(screenManager* manager, const Mouse* mouse)
{
    if (manager == NULL || mouse == NULL)
        return;

    if (!manager->active)
        return;

    switch (manager->currentScreen)
    {
        case screenIdMainMenu:
            screenManager_updateMainMenu(manager, mouse);
            break;

        case screenIdSettingsMenu:
            screenManager_updateSettingsMenu(manager, mouse);
            break;

        case screenIdControlsMenu:
            screenManager_updateControlsMenu(manager, mouse);
            break;

        case screenIdCreditsMenu:
            screenManager_updateCreditsMenu(manager, mouse);
            break;

        case screenIdGameSetupMenu:
            screenManager_updateGameSetupMenu(manager, mouse);
            break;

        case screenIdLocalPvPSetupMenu:
            screenManager_updateLocalPvPSetupMenu(manager, mouse);
            break;

        case screenIdEnginePvESetupMenu:
            screenManager_updateEnginePvESetupMenu(manager, mouse);
            break;
    }
}

void screenManager_updateKeyboard(screenManager* manager, const sfEvent* event)
{
    if (manager == NULL || event == NULL)
        return;

    if (!manager->active)
        return;

    switch (manager->currentScreen)
    {
        case screenIdLocalPvPSetupMenu:
            localPvPSetupMenu_updateKeyboard(manager->localPvPSetup, event);
            break;

        case screenIdEnginePvESetupMenu:
            enginePvESetupMenu_updateKeyboard(manager->enginePvESetup, event);
            break;

        default:
            break;
    }
}

void screenManager_setActive(screenManager* manager, sfBool active)
{
    if (manager == NULL)
        return;

    manager->active = active;
}

sfBool screenManager_isActive(const screenManager* manager)
{
    if (manager == NULL)
        return sfFalse;

    return manager->active;
}

void screenManager_setScreen(screenManager* manager, ScreenId screen)
{
    if (manager == NULL)
        return;

    screenManager_setOnlyActiveScreen(manager, screen);
}

ScreenId screenManager_getScreen(const screenManager* manager)
{
    if (manager == NULL)
        return screenIdMainMenu;

    return manager->currentScreen;
}

ScreenManagerRequest screenManager_getRequest(const screenManager* manager)
{
    if (manager == NULL)
        return screenManagerRequest_getNone();

    return manager->request;
}

ScreenManagerRequest screenManager_consumeRequest(screenManager* manager)
{
    ScreenManagerRequest request;

    if (manager == NULL)
        return screenManagerRequest_getNone();

    request = manager->request;
    manager->request = screenManagerRequest_getNone();

    return request;
}

void screenManager_draw(sfRenderWindow* window, const screenManager* manager)
{
    if (window == NULL || manager == NULL)
        return;

    if (!manager->active)
        return;

    switch (manager->currentScreen)
    {
        case screenIdMainMenu:
            mainMenu_draw(window, manager->main);
            break;

        case screenIdSettingsMenu:
            settingsMenu_draw(window, manager->settings);
            break;

        case screenIdControlsMenu:
            controlsMenu_draw(window, manager->controls);
            break;

        case screenIdCreditsMenu:
            creditsMenu_draw(window, manager->credits);
            break;

        case screenIdGameSetupMenu:
            gameSetupMenu_draw(window, manager->gameSetup);
            break;

        case screenIdLocalPvPSetupMenu:
            localPvPSetupMenu_draw(window, manager->localPvPSetup);
            break;

        case screenIdEnginePvESetupMenu:
            enginePvESetupMenu_draw(window, manager->enginePvESetup);
            break;
    }
}