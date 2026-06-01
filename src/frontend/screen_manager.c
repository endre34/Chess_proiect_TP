#include "frontend/screen_manager.h"

#include "frontend/screens/main_menu.h"
#include "frontend/screens/settings_menu.h"
#include "frontend/screens/credits_menu.h"
#include "frontend/screens/game_setup_menu.h"

#include <stdlib.h>

struct screenManager
{
    mainMenu* main;
    settingsMenu* settings;
    creditsMenu* credits;
    gameSetupMenu* gameSetup;

    ScreenId currentScreen;
    ScreenManagerRequest request;

    sfBool active;
};

static void screenManager_setOnlyActiveScreen(screenManager*, ScreenId);

static void screenManager_updateMainMenu(screenManager*, const Mouse*);
static void screenManager_updateSettingsMenu(screenManager*, const Mouse*);
static void screenManager_updateCreditsMenu(screenManager*, const Mouse*);
static void screenManager_updateGameSetupMenu(screenManager*, const Mouse*);

static void screenManager_setOnlyActiveScreen(screenManager* manager, ScreenId screen)
{
    mainMenu_setActive(manager->main, sfFalse);
    settingsMenu_setActive(manager->settings, sfFalse);
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

        case screenIdCreditsMenu:
            creditsMenu_setActive(manager->credits, sfTrue);
            break;

        case screenIdGameSetupMenu:
            gameSetupMenu_setActive(manager->gameSetup, sfTrue);
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
            /*
                Later: switch to a controls submenu.
                For now this intentionally does nothing.
            */
            break;

        case settingsMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);
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
            manager->request.type = screenManagerRequestStartGame;
            manager->request.data.gameSetup = gameSetupMenu_getData(manager->gameSetup);
            break;

        case gameSetupMenuActionVsEngine:
            manager->request.type = screenManagerRequestStartGame;
            manager->request.data.gameSetup = gameSetupMenu_getData(manager->gameSetup);
            break;

        case gameSetupMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);
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
    manager->credits = NULL;
    manager->gameSetup = NULL;

    manager->currentScreen = screenIdMainMenu;
    manager->request = screenManagerRequest_getNone();
    manager->active = sfTrue;

    manager->main = mainMenu_create(topLeft, bottomRight, resources);
    manager->settings = settingsMenu_create(topLeft, bottomRight, resources);
    manager->credits = creditsMenu_create(topLeft, bottomRight, resources);
    manager->gameSetup = gameSetupMenu_create(topLeft, bottomRight, resources);

    if (
        manager->main == NULL ||
        manager->settings == NULL ||
        manager->credits == NULL ||
        manager->gameSetup == NULL
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

    if (manager->credits != NULL)
        creditsMenu_destroy(manager->credits);

    if (manager->gameSetup != NULL)
        gameSetupMenu_destroy(manager->gameSetup);

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

        case screenIdCreditsMenu:
            screenManager_updateCreditsMenu(manager, mouse);
            break;

        case screenIdGameSetupMenu:
            screenManager_updateGameSetupMenu(manager, mouse);
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

        case screenIdCreditsMenu:
            creditsMenu_draw(window, manager->credits);
            break;

        case screenIdGameSetupMenu:
            gameSetupMenu_draw(window, manager->gameSetup);
            break;
    }
}